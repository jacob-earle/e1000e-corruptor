# A short shell script that will run all of the commands necessary to initiate a DMA transfer to a given location in physical memory in order to corrupt it
#!/bin/sh
./target_address.sh $1 $2 $3
make

# Disabling caching
sudo insmod cachedisable.ko

# Loading corrupted e1000e driver, which will perform the DMA transfer
sudo insmod e1000e-corruptor.ko

# Wait to allow the transfer to occur
sleep 20

# Remove the e1000e driver
sudo rmmod e1000e-corruptor

# Reenable caching
sudo rmmod cachedisable
