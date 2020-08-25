/* A simple kernel module that will disable caching on all processors when loaded and reenable it when unloaded.
This is achieved by setting the CD flag of the CR0 and disabling the MTRRs.
NOTE: The functionality of this kernel module should only be used within this project, as the cache is not invalidated when caching is disabled, so cache incoherencies may occur upon writes.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/smp.h> //contains the on_each_cpu function
#include <asm/msr.h>

// defining the identifying address of the IA32_MTRR_DEF_TYPE MSR, which will be used to disable all MTRRs
#define IA32_MTRR_DEF_TYPE 0x2ff

// function that will set the CD bit on the CR0 on the current CPU core, which will disable new values from being cached on that core
// code inspired by a StackOverflow post at https://stackoverflow.com/questions/3962950/how-to-set-control-register-0-cr0-bits-in-x86-64-using-gcc-assembly-on-linux
// in order to maintain cache coherency, a WBINVD instruction should be used after moving the new value to the CR0
void disable_cr0(void){
    asm volatile("pushq  %rax;\n\t"
                "movq    %cr0,%rax;\n\t"
                "orq    $(1 << 30),%rax;\n\t"
                "movq    %rax,%cr0;\n\t"
                "popq   %rax;"
    );
}

// function that will disable the MTRRs on the current core by clearing the E flag on the IA32_MTRR_DEF_TYPE MSR on the current core
// the E flag is located at bit 11 of the IA32_MTRR_DEF_TYPE
void disable_mtrr(void){
    // reading value from MTRR
    unsigned long long val = native_read_msr(IA32_MTRR_DEF_TYPE);

    // clearing the E flag to disable caching 
    val = val & (~(1 << 11));
    
    //writing back to the MTRR
    wrmsrl(IA32_MTRR_DEF_TYPE, val);
}

// function that reenables caching by resetting the CD flag on the CR0 register
void reenable_cr0(void){
    asm volatile("pushq  %rax\n\t"
                "movq    %cr0,%rax;\n\t"
                "andq    $(~(1 << 30)),%rax;\n\t"
                "movq    %rax,%cr0;\n\t"
                "popq   %rax"
    );
}

// function that resets the E flag on the IA32_MTRR_DEF_TYPE MSR, which will reenable caching according the types defined in the MTRRs
void reenable_mtrr(void){
    // reading value from MTRR
    unsigned long long val = native_read_msr(IA32_MTRR_DEF_TYPE);

    // clearing the E flag to disable caching 
    val = val | (1 << 11);
    
    //writing back to the MTRR
    wrmsrl(IA32_MTRR_DEF_TYPE, val);
}

// function that will disable caching on a single core by calling disable_cr0 and disable_mtrr on the current cpu
void disable_cache_on_single_core(void * info){
    disable_cr0();
    disable_mtrr();
}

// function that will reenable caching on a single core by calling reenable_mtrr and reenable_cr0 on the current cpu
void reenable_cache_on_single_core(void * info){
    reenable_mtrr();
    reenable_cr0();
}

// init function for the kernel module
// when the kernel module is loaded, it will call disable_cache_on_single_core on every cpu core to disable caching across the entire cpu
int init_module(void){
    printk(KERN_INFO "Loading kernel module and disabling caching.\n");
    on_each_cpu(disable_cache_on_single_core, NULL, 1);
    return 0;
}

// unloading function for the kernel module
// when the kernel module is unloaded, it will call reenable_cache_single_core on every cpu core to reenable caching accross the entire cpu
void cleanup_module(void){
    on_each_cpu(reenable_cache_on_single_core, NULL, 1);
    printk(KERN_INFO "Unoading kernel module and reenabling caching.\n");
}

MODULE_LICENSE("GPL");