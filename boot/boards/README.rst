You can place per-board configuration here. See the comments in the below for context of the 
overlay file found here: 

/ {
    chosen { 
    //[MPORTANT] - must change and configure for which slot you want code to be 
    // uploaded to use west
        zephyr,flash = &flash0;
	zephyr,code-partition = &boot_partition;
    };
};

/*
 * This overlay add MCUboot partitions to the flash memroy, which is required to use
 * MCUboot as our chosen bootloader. Adjust partitions as needed but note that boot, slot0,
 * and slot1 are required and must be contiguous if using MCUboot as stage 1 bootloader.
 */
&flash0 {                                   //256 KB Flash
    /*
     * Partition layout for MCUboot:
     * - boot_partition: MCUboot itself
     * - slot0_partition: Active firmware image
     * - slot1_partition: Backup firmware image (for updates)
     * - scratch_partition: Used by MCUboot during updates
     * - storage_partition: For persistent data storage
     */
     partitions {
        compatible = "fixed-partitions";
        #address-cells = <1>;
        #size-cells = <1>;

        //boot paritions required by MCUboot
        boot_partition: partition@0 {
            label = "mcuboot";
            // reg = <address length>
            reg = <0x00000000 0x8000>;          //32 KB
        };
        slot0_partition: partition@8000{
            label = "image-0";
            reg = <0x00008000 0x18000>;         //96 KB
        };
        slot1_partition: partition@20000{
            label = "image-1";
            reg = <0x00020000 0x18000>;        //96 KB
        };
        //scratch_partition: partition@38000{
        //    label = "image-scratch";
        //    reg = <0x00038000 0x6800>;      //26 KB -> Implement later if needed.
        //};

        /*
         * Storage partition is commented out so overlay does not over-write
         * the default settings found in the default board.dts file. This could be
         * adjusted as needed. [Note] The storage_partition node is defined for use by
         * a file system or other nonvolatile storage API.
         * See Zephyr Docs -> OS Services/Storage/Flash map
         *
         */
        // storage_partition: partition@3e800 {
        //     label = "storage";
        //     reg = <0x0003e800 0x1800>; // 6 KB
        // };
     };
};

