#ifndef __SSD_DEFINITION__
#define __SSD_DEFINITION__
#include <vector>

//-------------------------------------------------------------------------------------------------
enum ENUM_FIRMWARE_MODE {UNKNOW_MODE, ROM_SAFE_MODE, BURNER_MODE, RDT_FW_MODE, PRODUCT_FW_MODE};
enum ENUM_READ_LOG_EXT_MODE {READ_LOG_EXT_NONE_SEGMENT, READ_LOG_EXT_SEGMENT};

//
// for the Upgrade_FW_By_Burner of MP_USE_DATA_CONFIG_INFO structure
// 0: normal MP flow
// 1: use MP tool to upgrade fw by burner
// 2: use AP tool to upgrade fw by burner
//
enum ENUM_UPGRADE_FW_FLOW {MP_NORMAL_FLOW, MP_UPGRADE_BY_BURNER, AP_UPGRADE_BY_BURNER};


const char    ROM_SAFE_MODE_MODEL_NUMBER[]       = "MRVL TSATA PART0";     // model number of rom safe mode
const char    RDT_FW_BOOT_MODEL_NUMBER[]         = "RDT";                  // model number of RDT firmware boot
const char    DOWNLOAD_BURNER_DONE_SIGNATURE_1[] = "Burner";               // a part of model number of download burner done
const char    DOWNLOAD_BURNER_DONE_SIGNATURE_2[] = "uBnrre";               // a part of model number of download burner done --> fw forget reverse high / low byte
const char    IMAGE_HEADER_SIGNATURE[]           = "SSS_CONF";
const BYTE    JEDEC_SIGNATURE[]                  = {0x4A, 0x45, 0x53, 0x44};
const char    STATUS_A0_SIGNATURE[]              = "3SSTATUS";             // burner status 0xA0 signature

const char    EC_HEADER[] = "ECT_DATA";  // erase count table header
const char    FD_HEADER[] = "FCT_DATA";  // factory defect table header
const char    GD_HEADER[] = "DFT_DATA";  // grown defect table header

const UINT32  EC_ADDR = 0x000080DE;  // erase count table address. read and write log ext address
const UINT32  FD_ADDR = 0x000000DE;  // factory defect table address. read and write log ext address
const UINT32  GD_ADDR = 0x000040DE;  // grown defect table address. read and write log ext address



#define  MAX_CH            (2)
#define  MAX_CE            (8)
#define  MAX_LUN           (4)
#define  MAX_PLANE         (4)

#define  MODEL_NUMBER_LEN  (40)  // unit: char
#define  SN_LEN            (20)  // unit: char

#define  FD_TABLE_SIZE     (32)  // factory defect table size.  unit: sector
#define  GD_TABLE_SIZE     (32)  // grown defect table size.  unit: sector
#define  EC_TABLE_SIZE     (32)  // erase count table size.  unit: sector

#define  FLASH_ID_LENGTH   (6)
#define  NAND_MAP_SIZE     (64)  // unit: udev_addr_t

#define  SMART_TABLE_SIZE  (50)  // number of SMART ID

#define  FLASH_ID_SIZE     (8)   // number of Flash ID


//
// LOG_EXT_CONFIG_A0 512 bytes (burner status)
//
// SSS_Flag:
#define BNR_NAND_PHY_CALI_FAIL         (_BIT8)    // nand py calibration fail
#define BNR_READ_BACK_ERR              (_BIT9)    // write block info fail
#define BNR_F_ROM_IMG_GOT              (_BIT10)   // get ROM image
#define BNR_D_ERASE_TBL_DONE           (_BIT11)   // erase count table download doen
#define BNR_D_LAT_DEF_TBL_DONE         (_BIT12)   // late defect table download done
#define BNR_D_FDFT_TBL_DONE            (_BIT13)   // factory defetct table download done
#define BNR_I_INITIAL_DONE             (_BIT14)   // initial device done
#define BNR_E_ERASE_DONE               (_BIT15)   // erase done flag
#define BNR_F_FW_GOT                   (_BIT17)   // got FW image
#define BNR_F_TL_GOT                   (_BIT18)   // got TL image
#define BNR_F_MR_FOUND                 (_BIT19)   // Found MR
#define BNR_F_REQ_FORCED_ABORT         (_BIT20)   // SATA Plug-out
#define BNR_F_NV_ERR_NO_MEM            (_BIT21)   // original burner_start err flag    // Burner_Start_Run_Status will be done, we need to check this flag
#define BNR_F_NV_ERR_WERU0_ERR         (_BIT22)   // original burner_start err flag    // Burner_Start_Run_Status will be done, we need to check this flag
#define BNR_F_NV_ERR_VIRGIN            (_BIT23)   // Virgin flag (new card or no_MR)
#define BNR_F_TOO_MANY_DEFECT          (_BIT24)   // too many defect flag              // Burner_Start_Run_Status will be done, we need to check this flag
#define BNR_F_FE_IMG_GOT               (_BIT29)   // get FE image
// Burner_Start_Run_Status:
#define BNR_F_STAGE_0                  (0x80)
#define BNR_F_STAGE_1                  (0x40)
#define BNR_F_STAGE_2                  (0x20)
#define BNR_F_STAGE_2_FORCE_ERASE      (0x21)
#define BNR_F_STAGE_2_DF_SCAN          (0x22)
#define BNR_F_STAGE_2_LATER_DF_SCAN    (0x23)
#define BNR_F_STAGE_2_ERASE_SAFE       (0x24)
#define BNR_F_STAGE_2_ERASE_NOSAFE     (0x25)
#define BNR_F_STAGE_2_PRE_PROGRAM      (0x26)
#define BNR_F_STAGE_3                  (0x10)
#define BNR_F_STAGE_4                  (0x08)
#define BNR_F_DONE                     (0xFF)
#define BNR_F_ERR                      (0x00)


//
// RDT
//
enum
{
    RDT_RESULT_PASS,         // 0, no error
    RDT_RESULT_UNC_FAIL,     // 1, Uncorrectable ECC Error
    RDT_RESULT_PRG_FAIL,     // 2, Program Status Failed
    RDT_RESULT_ER_FAIL,      // 3, Erase Status failed
    RDT_RESULT_OV_THR_FAIL,  // 4, Ecc Error bits count is over the threshold
    RDT_RESULT_ORG_DEFECT,   // 5, original defect block
    RDT_RESULT_IO_FAILS,     // 6, flash I/O error
    RDT_RESULT_OTHER_FAIL    // 7, other faild
};

#define MAX_RESULT_COUNT    ((16384 - 128) / 16)

const char RDT_RESULT_FULL_DIE_TLC_SIGNATURE[]    = "TF";  // TLC full die
const char RDT_RESULT_FULL_DIE_SLC_SIGNATURE[]    = "SF";  // SLC full die
const char RDT_RESULT_SAMPLING_TLC_SIGNATURE[]    = "TS";  // TLC sampling
const char RDT_RESULT_SAMPLING_SLC_SIGNATURE[]    = "SS";  // SLC sampling
const char RDT_RESULT_CYCLE_INFO_SIGNATURE[]      = "CY";  // clycle info
const char RDT_RESULT_POWER_CYCLE_SIGNATURE[]     = "PC";  // power cycle
const char RDT_RESULT_FLASH_IO_TEST_SIGNATURE1[]  = "FB";  // flash IO test
const char RDT_RESULT_FLASH_IO_TEST_SIGNATURE2[]  = "BF";  // flash IO test


//
// _RDT_INI_INFO
//
const char RDT_INI_INFO_SIGNATURE[] = "RDT_INI";

#define RDT_INI_INFO_CONTENT_LENGTH  (4096)


//
// WPB0 status
//
#define CHK_BLK_INFO_OK         (_BIT0)
#define CHK_BLK_INFO_GET_ERR    (_BIT1)
#define CHK_BLK_INFO_CRC_ERR    (_BIT2)
#define CHK_BLK_INFO_SIG_ERR    (_BIT3)
#define CHK_BLK_INFO_ERR        (_BIT7)


//
// signature for UI parse log content
//
const char SIGNATURE_FOR_UI_PARSE[] = "For_Parse_Msg";


//
// SStatus IPM (Interface Power Management) state
//
#define IPM_DEV_NOT_PRESETNT  (0)
#define IPM_ACTIVE_STATE      (1)
#define IPM_PARTIAL_STATE     (2)
#define IPM_SLUMBER_STATE     (6)
#define IPM_DEV_SLEEP_STATE   (8)


//-------------------------------------------------------------------------------------------------
#pragma pack(1)

// image header 64 bytes
typedef struct _IMAGE_HEADER_CONFIG_INFO
{
    char    Signature[8];
    UINT32  Version;
    UINT32  Select_Bit;
    UINT32  Auth;
    UINT32  Encrypt;
    UINT32  Num_Of_Section;
    UINT32  Section_Hdr_Size;
    UINT32  Section_Table_Offset;
    UINT32  Flash_Addr;
    UINT32  CRC;
    UINT32  Image_Size;
    BYTE    Image_Type;
    BYTE    Rsv[7];
    UINT16  Ver_Major;
    UINT16  Ver_Minor;
    UINT16  Ver_OEM;
    UINT16  Ver_Build;
} IMAGE_HEADER_CONFIG_INFO, *PIMAGE_HEADER_CONFIG_INFO;


// NANA PHY calibration 184 bytes
typedef struct _NAND_PHY_CALIBRATION_CONFIG_INFO
{
    BYTE  Rsv[184];
} NAND_PHY_CALIBRATION_CONFIG_INFO, *P_NAND_PHY_CALIBRATION_CONFIG_INFO;


// SATA PHY patch 264 bytes
typedef struct _SATA_PHY_PATCH_CONFIG_INFO {
    BYTE    Signature[4];
    UINT32  Size;
    BYTE    Start[256];
} SATA_PHY_PATCH_CONFIG_INFO, *PSATA_PHY_PATCH_CONFIG_INFO;


// ATA identify config 512 bytes
typedef struct _ATA_IDENTIFY_CONFIG_INFO
{
    BYTE    Rsv1[14];
    BYTE    CFA[6];
    char    Serial_Number[SN_LEN];
    BYTE    Rsv2[6];
    char    Firmware_Revision[8];  // WORD[23] ~ WORD[26]
    char    Model_Number[MODEL_NUMBER_LEN];
    BYTE    Rsv3[4];
    UINT16  Capabilities;
    UINT16  Capabilities_Standby_Time;
    BYTE    Free_Fall_Sensitivity_And_Reported[6];
    BYTE    Cylinder_Head_Sec_Per_Track[6];
    BYTE    Rsv4[4];
    UINT16  Support_Conf;
    UINT32  Total_Number_Of_LBA;  // WORD[60] ~ WORD[61]
    BYTE    Rsv5[6];
    UINT64  Transfer_Cycle_Time;
    BYTE    Rsv6[12];
    UINT16  NCQ_Depth;
    UINT16  SATA_Capabilities;  //WORD[76]
    UINT16  STAT_Additional_Capabilities;
    UINT16  SATA_Features_Supported;  //WORD[78]
    BYTE    Rsv7[6];
    BYTE    Command_And_Feature_Sets_Supported[6];
    BYTE    Command_And_Feature_Sets_Supported_Or_Enabled[6];
    BYTE    Rsv8[12];
    UINT16  Vendor_Recommended_Acoustic_Management_Value;
    BYTE    Rsv9[10];
    UINT64  Number_Of_User_Addressable_Logical_Sectors;  // WORD[100] ~ WORD[103]
    BYTE    Rsv10[4];
    UINT16  Physical_Sector_Size;
    UINT16  Inter_Seek_Delay;
    BYTE    WWN[8];  // world wide name. WORD[108] ~ WORD[111]
    BYTE    Rsv11[14];
    UINT32  Command_And_Feature_Sets_Supported_2;
    BYTE    Rsv12[16];
    BYTE    Vendor_Specific[62];
    BYTE    RSV_CFA[16];
    UINT16  Device_Nominal_From_Factor;
    BYTE    Rsv13[2];
    UINT64  Additional_Product_Identifier;
    BYTE    Rsv14[4];
    char    Current_Media_Serial_Number[60];
    UINT16  SCT_Command_Transport;
    BYTE    Rsv15[46];
    UINT64  Extended_Number_Of_User_Addressable_Sectors;
    UINT16  Min_Number_Of_512_Bytes_Data_Blocks;
    UINT16  Max_Number_Of_512_Bytes_Data_Blocks;
    BYTE    Rsv16[38];
    UINT16  CheckSum;

} ATA_IDENTIFY_CONFIG_INFO, *PATA_IDENTIFY_CONFIG_INFO;


// for Marvell use 512 bytes
typedef struct _MARVELL_AREA_CONFIG_INFO
{
	UINT32 User_Capacity_Size;              // for 3s FW only
    BYTE   PMU_Switch;                      // PMU
    BYTE   OP7_Enable;                      // OP_Option
    BYTE   Device_Sleep_Enable;             // Device_Sleep_Enable
    BYTE   HIPM_Enable;                     // HIPM_eanble
    BYTE   DIPM_Enable;                     // DIPM_enable
    BYTE   SMART_Switch;                    // SMART_Table_form
    BYTE   Rsv1[2];
    UINT32 PE_Count;                        // Target_PE_Count_TLC
    UINT32 User_FW_Version;                 // User_FW_Revision
    BYTE   Enable_Empty_Page_Read_Retry;    // bypass empty page read retry
    BYTE   Enable_Power_cycle_Patch;        // enable power cycle patch
    BYTE   Rsv2[490];
	
} MARVELL_AREA_CONFIG_INFO, *PMARVELL_AREA_CONFIG_INFO;


// JEDEC 512 bytes
typedef struct _JEDEC_DATA_CONFIG_INFO
{
    BYTE    Parameter_Page_Signature_JESD[4];
    UINT16  Revision_Number;
    UINT16  Feature_Supported;
    BYTE    Optional_Commands_Supported[3];
    UINT16  Secondary_Commands_Supported;
    BYTE    Number_Of_Parameter_Pages;
    BYTE    Rsv1[18];
    BYTE    Device_Manufacturer[12];
    BYTE    Device_Model[20];
    BYTE    JEDEC_Manufacturer_ID[6];
    BYTE    Rsv2[10];    
    UINT32  Number_Of_Data_Bytes_Per_Page;
    UINT16  Number_Of_Spare_Bytes_Per_Page;
    BYTE    Rsv3[6];
    UINT32  Number_Of_Pages_Per_Block;
    UINT32  Number_Of_Blocks_Per_Logical_Unit;
    BYTE    Number_Of_Logical_Units;
    BYTE    Number_Of_Address_Cycles;
    BYTE    Number_Of_Bit_Per_Cell;
    BYTE    Number_Of_Programs_Per_Page;
    BYTE    Multi_Plane_Addressing;
    BYTE    Multi_Plane_Operation_Attributes;
    BYTE    Rsv4[38];        
    UINT16  Asynchronous_SDR_Speed_Grade;
    UINT16  Toggle_DDR_Speed_Grade;
    UINT16  Synchronous_DDR_Speed_Grade;    
    BYTE    Asynchronous_SDR_Features;
    BYTE    Toggle_Mode_DDR_Features;
    BYTE    Synchronous_DDR_Features;
    UINT16  tPROG_Max_Page_Program_Time;
    UINT16  tBERS_Max_Block_Erase_Time;
    UINT16  tR_Max_Page_Read_Time;
    UINT16  tR_Max_Multi_Plane_Page_Read_Time;
    UINT16  tCSS_Min_Change_Column_Setup_Time;
    UINT16  IO_Pin_Capacitance;
    UINT16  Input_Pin_Capacitance;
    UINT16  CK_Pin_Capacitance;
    BYTE    Driver_Strength_Support;
    UINT16  tADL_Program_Page_Register_Clear_Enhancement_tADL_Value;
    BYTE    Rsv5[36];
    BYTE    Guaranteed_Valid_Blocks_Of_Target;
    UINT16  Block_Endurance_For_Guaranteed_Valid_Blocks;    
    UINT64  ECC_And_Endurance_Information_Block_0;
    UINT64  ECC_And_Endurance_Information_Block_1;
    UINT64  ECC_And_Endurance_Information_Block_2;
    UINT64  ECC_And_Endurance_Information_Block_3;
    BYTE    Rsv6[177];
    UINT16  Vendor_Specific_Revision_Number;
    BYTE    Vendor_Specific[88];
    UINT16  CRC;
} JEDEC_DATA_CONFIG_INFO, *PJEDEC_DATA_CONFIG_INFO;


typedef struct _cfg_tbl_nand_info_t
{
    //spb_top_mgr_t
    UINT16  nr_werus;

    //spb_cfg_t
    BYTE    width;              // actual width, # of udev, used for space management

   //dev_geo_t;
    BYTE    nr_bits_per_cell;   // Cell type
    BYTE    nr_luns;            // = 2 ^ rl.lun_bits
    BYTE    nr_planes_per_lun;  // = 2 ^ rl.plane_bits
    UINT16  page_data_sz;
    UINT16  page_spare_sz;

    /*
    * nr_blocks_per_plane is not necessarily power of 2, some NAND has extra
    * blocks, which is good so that we can have more spare blocks.
    * a.k.a., nr_blocks_per_plane != 2 ^ rl.block_bits
    */
    UINT16  nr_blocks_per_die;

    /*
    * nr_pages_per_block is not ncesessarily power of 2, so far we have seen this
    * on TLC NAND. a.k.a., nr_pages_per_block != 2 ^ rl.page_bits
    */
    UINT16  nr_pages_per_block;
    BYTE    support_ce;
    BYTE    support_channel;
    BYTE    rsv[16];
} cfg_tbl_nand_info_t;


// RDT config 2048 bytes
typedef struct _RDT_CONFIG_INFO
{
    BYTE   Rsv1[256];

    // full die RDT
    UINT32  TLC_Cycle;
    UINT32  SLC_Cycle;
    BYTE    Rsv2[248];

    // test condition
    UINT32  Mark_Bad;
    UINT32  MP_Test;
    UINT32  Preload_Data;
    UINT32  Bad_Sample_Black_Per_CE;
    BYTE    Rsv3[240];

    // TLC sample block RDT
    UINT32  TLC_Dwell_Time;
    UINT32  TLC_Sampling_Cycle;
    UINT32  TLC_ECC_Log_Interval;
    UINT32  TLC_Start_Select_Block;
    UINT32  TLC_End_Select_Block;
    UINT32  TLC_Random_Block;
    UINT32  TLC_Sample_Block_Quantity;
    UINT32  TLC_Sampling_ECC_Bit_Threshold;
    BYTE    TLC_Sampled_WERU_Bitmap[181];
    BYTE    Rsv4[43];

    // SLC sample block RDT
    UINT32  SLC_Special_Test_Cycle;
    UINT32  SLC_ECC_Log_Interval;
    UINT32  SLC_Random_Block;
    UINT32  SLC_Block_Quantity;
    UINT32  SLC_Start_Select_Block;
    UINT32  SLC_End_Select_Block;
    UINT32  SLC_Sampling_ECC_Bit_Threshold;
    BYTE    SLC_Sampled_WERU_Bitmap[181];
    BYTE    Rsv5[3];
    UINT32  SLC_Dwell_Time;
    BYTE    LED_Mode;      // 0: for KDI, 1: for ADATA.
    BYTE    Pattern_Mode;  // 0: 0x55AA 0xAA55 ...  1: 0x84210 ...
    BYTE    SLC_Full_Die_ECC_Log_Interval;
    BYTE    TLC_Full_Die_ECC_Log_Interval;
    UINT32  RDT_GD_TH;       // if set 0, fw won't check
    BYTE    LED_Light_Time;  // (LED_Light_Time + 1) * 0.1 second
    BYTE    LED_Dark_Time;   // (LED_Dark_Time + 1) * 0.1 second
    BYTE    LDPC_Switch;     // 1: turn off LDPC  0: original status (LDPC on)
    BYTE    Full_Die_Deep_Read_Retry_Option;  // 1: full die test turn on Deep Read Retry, 2: full die test turn off deep read retry
    UINT16  Full_Die_Erase_Count_Threshold;   // if weru's erase count >= Erase_Count_Threshold, RDT will by pass this weru when TLC full die test
    UINT16  Bypass_SLC_Weru_For_TLC_Full_Die_Test;
    UINT32  TLC_Full_Die_ECC_Bit_Threshold;
    UINT32  SLC_Full_Die_ECC_Bit_Threshold;
    UINT32  Sorting_Board_Key_Count;          // assign key count for sorting board
    UINT32  Sorting_Board_Current_Key_Count;  // for read back sorting board current key count
    BYTE    RR_Pass_Add_To_GD;                // condition: normal read verify fail (LDPC) --> ECC is under ECC threshold --> read re-try pass
                                              // 1: add this block to GD, 0: won't add to GD
    BYTE    Option1;
    BYTE    Option2;
    BYTE    Option3;
    
    UINT16  Read_Page_Index;    // For RDT Preload 5, read distribution test option
    UINT16  Read_Count;         // For RDT Preload 5, read distribution test option

    BYTE    Sorting_Board_Flash_Count;       // offset 0xd00
    BYTE    Rsv7[3];
    BYTE    Flash_IO_Test_Flag;              // enable or disable flash IO test, offset 0xd04
    BYTE    Rsv8[3];
    UINT32  Flash_IO_Test_Count;             // flash IO test count, offset 0xd08
    BYTE    Rsv9[244];
    BYTE    Rsv10[256];
    cfg_tbl_nand_info_t  cfg_tbl_nand_info;
    BYTE    Rsv11[224];

} RDT_CONFIG_INFO, *PRDT_CONFIG_INFO;


typedef struct _RDT_INI_INFO
{
    char    Signature[16];  // signature --> "RDT_INI"
    char    File_Name[512];
    UINT32  Content_Size;
    char    Content[4096];  // INI content
} RDT_INI_INFO, *PRDT_INI_INFO;


//
// LOG_EXT_CONFIG_A0 512 bytes (burner status), SSS_Flag and Burner_Start_Run_Status are defined above
//
typedef struct _phy_cali_result
{
    BYTE  min;
    BYTE  max;
    BYTE  avg;
    BYTE  rsv;
} phy_cali_result;

typedef struct _toggle_cali_result
{
    phy_cali_result toggle_result[12];
    /*
    mode -> ch -> falling / rising
    mode2 , ch0 , falling: index=0
    mode2 , ch0 , rising : index=1
    mode2 , ch1 , falling: index=2
    mode2 , ch1 , rising : index=3
    mode3 , ch0 , falling: index=4
    mode3 , ch0 , rising : index=5
    mode3 , ch1 , falling: index=6
    mode3 , ch1 , rising : index=7
    mode4 , ch0 , falling: index=8
    mode4 , ch0 , rising : index=9
    mode4 , ch1 , falling: index=10
    mode4 , ch1 , rising : index=11
    */
} toggle_cali_result;

typedef struct _LOG_EXT_CONFIG_A0
{
    UINT32               Marvell_Flag;
    UINT32               SSS_Flag;                 // reference this variable to check burner status flag
    BYTE                 Percentage;               // percentage for each step (each step of burner start and call vendor safe erase will update)
    BYTE                 Burner_Start_Run_Status;
    BYTE                 Total_Percentage;         // percentage for all step (burner start total step)
    BYTE                 Last_Burner_Run_Stage;
    UINT16               Num_Factory_DF_Blk;       // factory defect number
    UINT16               Num_Grown_DF_Blk;         // grown defect number
    BYTE                 Flash_ID[8][8];
    toggle_cali_result   Nand_Phy_Cali_Result;
    BYTE                 Rsv1[124];
    BYTE                 Flash_Shallow[8];         // B16 material version, 1 : 2rd, < 8/24, 2 : 3rd, > 8/24
    BYTE                 Status_Sig[8];            // signature: 3SSTATUS
    UINT32               Main_Error;               // SSS_Flag fail flag
    BYTE                 Binary_String[208];
    cfg_tbl_nand_info_t  cfg_tbl_nand_info;
} LOG_EXT_CONFIG_A0, *PLOG_EXT_CONFIG_A0;

// MP record 
typedef struct _MP_Record
{
    BYTE  MP_Start_Time[12];        // record MP start time
    BYTE  MP_Stage[3];              // record MP stage FTA or FTB
    BYTE  MP_Flow[8];               // record FTB flow
    bool  Is_Force_Erase;           // record MP is force erase
    BYTE  Rsv[8];
} MP_RECORD, *PMP_RECORD;

// MP evnet table
typedef struct _MP_Event_Table
{
    BYTE        MP_Record_Index;        // MP record index, max = 10
    BYTE        Rsv2[15];
    MP_RECORD   MP_Record[10];          // 10 MP records
} MP_EVENT_TABLE, *PMP_EVENT_TABLE;

// MP use data 12 KBytes
typedef struct _MP_USE_DATA_CONFIG_INFO
{
    char  Previous_Stage[16];         // record stage: FTA, FTB, or QC
    char  Current_Stage[16];          // record stage: FTA, FTB, or QC

    char  MP_Start_Time[64];
    char  MP_Version[64];
    char  FW_Version[16];
    char  INI_File_Name[512];

    RDT_INI_INFO  RDT_INI_Info;       // structure size --> 4628 bytes
    char  Burner_File_Name[256];

    LOG_EXT_CONFIG_A0  A0_Info;
    BYTE  Upgrade_FW_By_Burner;       // record that use what flow to upgrade firmware. refer to ENUM_UPGRADE_FW_FLOW
    char  FW_File_Name[256];          // combo firmware file name
    
    BYTE  Timing_Mode;                // record toggle speed or ONFI speed
    BYTE  Rsv1[10];                   // to let KDI_KeyPro_User_Data align 16 byte
    BYTE  KDI_KeyPro_User_Data[512];  // 0x28D0, save Kingston KeyPro user data that is encrypted by AES
    BYTE  KDI_XOR_Key[1024];          // 0x2AD0, save Kingston XOR key define in INI file
    
    MP_EVENT_TABLE MP_Event_Table;    // 0x2ED0, Record MP event
    
    BYTE  Rsv[12288 - 8224];
} MP_USE_DATA_CONFIG_INFO, *PMP_USE_DATA_CONFIG_INFO;


// LOG_EXT_CONFIG_B0 16 KBytes
typedef struct _LOG_EXT_CONFIG_B0
{
    IMAGE_HEADER_CONFIG_INFO          Image_Header_Config;          //64 bytes
    NAND_PHY_CALIBRATION_CONFIG_INFO  NANA_PHY_Calibration_Config;  //184 bytes
    SATA_PHY_PATCH_CONFIG_INFO        SATA_PHY_Patch_Config;        //264 bytes
    ATA_IDENTIFY_CONFIG_INFO          ATA_Identify_Config;          //512 bytes
    MARVELL_AREA_CONFIG_INFO          Marvell_Area_Config;          //512 bytes
    JEDEC_DATA_CONFIG_INFO            JEDEC_Data_Config;            //512 bytes
    RDT_CONFIG_INFO                   RDT_Config;                   //2048 bytes
    MP_USE_DATA_CONFIG_INFO           MP_Use_Data_Config;           //12 KBytes

} LOG_EXT_CONFIG_B0, *PLOG_EXT_CONFIG_B0;


typedef struct _DEFECT_ADDR
{
    std::vector<UINT16>  Defect_Block;
} DEFECT_ADDR, *PDEFECT_ADDR;


typedef struct _LUN_INFO
{
    DEFECT_ADDR Plane[MAX_PLANE];
} LUN_INFO, *PLUN_INFO;


typedef struct _CE_INFO
{
    LUN_INFO Lun[MAX_LUN];
} CE_INFO, *PCE_INFO;


typedef struct _CH_INFO
{
    CE_INFO Ce[MAX_CE];
} CH_INFO, *PCH_INFO;


typedef struct _DEFECT_INFO
{
    CH_INFO Ch[MAX_CH];
} DEFECT_INFO, *PDEFECT_INFO;

// EC 
typedef struct _ERASE_COUNT_INFO
{
    int WERU_No;
    int WERU_Erase_Count;
} ERASE_COUNT_INFO, *PERASE_COUNT_INFO;

// SMART data condition
typedef struct _SMART_DATA_CONDITION
{
    int     ID;
    UINT64  Max;
    UINT64  Min;
} SMART_DATA_CONDITION, *PSMART_DATA_CONDITION;

// SMART Data
typedef struct _SMART_DATA
{
    int	  ID;
	BYTE  Raw_Value[6];
	char  Raw_Value_Result[5];
} SMART_DATA, *PSMART_DATA;


// EC, FD, GD table header (64 bytes)
typedef struct _image_hdr
{
    char    signature[8];
    UINT32  version;
    UINT32  select_bit;
    UINT32  auth;
    UINT32  encrypt;
    UINT32  num_of_section;
    UINT32  section_hdr_size;
    UINT32  section_table_offset;
    UINT32  flash_addr;
    UINT32  crc;
    UINT32  image_size;

    // the below field is supported since Image Creator 1.0.0.1020
    char    image_type;     /* image type: SINGLE/DUAL_SAME/DUAL_DIFF 48*/
    char    reserved[7];    /* reserved for future use 49-55 */
    short   ver_major;      /* improved version - Major 56-57 */
    short   ver_minor;      /* improved version - Minor 58-59 */
    short   ver_oem;        /* improved version - OEM 60-61 */
    short   ver_build;      /* improved version - Build 62-63 */
} image_hdr;


// 32 bytes
typedef struct _sect_hdr
{
    char    section_name[16];
    UINT32  load_addr;
    UINT32  size;
    UINT32  section_data_offset;
    UINT32  flash_addr;
} section_hdr;


//
// RDT result structure (from FW team)
//
typedef union _RDT_BLOCK_RESULT
{
    struct
    {
        UINT16  CH       :4;                 //
        UINT16  CE       :4;                 //
        UINT16  Die      :4;
        UINT16  Plane    :4;                 // 2 Byte
        UINT16  Block;                       // 2 Byte  err position

        UINT16  Cycle;                       // 2 Byte
        UINT16  RR_Error_Bit;                // 2 Byte    Max_tPro redefined to RR_Error_Bit
        UINT16  Raw_bit_Error;               // 2 Byte
        BYTE    weru_hi;                     // 1 Byte
        BYTE    Retry_Count;                 // 1 Byte
        UINT16  Temp;                        // 2 Byte   (K)
        BYTE    weru_lo;                     // 1 Byte
        BYTE    Failed_Type;                 // 1 Byte
    } rec;
    BYTE rev[16];
} RDT_BLOCK_RESULT;


//
// cycle time result structure (from FW team)
//
typedef union _RDT_C_RESULT
{
    struct 
    {   
        UINT32  g_cycle;                     // 4 Byte
        UINT16  max_temp;                     // 2 Byte, MAX. temperature (C)
        UINT16  min_temp;                     // 2 Byte, MIN. temperature (C)      
        UINT32  signature;                   // 4 Byte  byte#0/#1 record TF, SF, SS, TS
        UINT32  duration_time;               // 4 Byte. unit: 100 ms
        //u32 end_time;                    // 4 Byte
    } rec;
    BYTE rev[16];
} RDT_C_RESULT;


typedef struct _RDT_RESULT 
{
    union
    {
        struct
        {
            UINT32  signature;                   // 4 Byte  byte#0/#1 record TF, SF, SS, TS or CY
            UINT32  cycle;                       // 4 Byte, Result cycle
            UINT32  version;                     // 4 Byte
            //u8   rev[3];                        //
            UINT16  max_temp;                    // 2 Byte, MAX. temperature (C)
            UINT16  min_temp;                    // 2 Byte, MIN. temperature (C)
            UINT32  count;                       // 4 Byte
            //UINT32  start_time;                  // 4 Byte
            UINT32  RDT_FW_Use;                  // MP can't use it (for FW)
            UINT32  end_time;                    // 4 Byte
            //u32  last_mu_off;
            //u32  rdt_rec_indx;

            // time stamp of TLC Full Did
            UINT32  tlc_full_die_cycle;          // 4 Bytes, total cycle of TLC Full Die Testing
            UINT32  tlc_full_die_timestamp;      // 4 Bytes, Time Stamp of TLC Full Die Testing  // record total cycle done

            // time stamp of SLC Full Did
            UINT32  slc_full_die_cycle;          // 4 Bytes, total cycle of SLC Full Die Testing
            UINT32  slc_full_die_timestamp;      // 4 Bytes, Time Stamp of SLC Full Die Testing  // record total cycle done

            // time stamp of TLC Sampling Did
            UINT32  tlc_sampling_cycle;          // 4 Bytes, total cycle of TLC Sampling Testing
            UINT32  tlc_sampling_timestamp;      // 4 Bytes, Time Stamp of TLC Sampling Testing  // record total cycle done

            // time stamp of SLC Sampling Did
            UINT32  slc_sampling_cycle;          // 4 Bytes, total cycle of SLC Sampling Testing
            UINT32  slc_sampling_timestamp;      // 4 Bytes, Time Stamp of SLC Sampling Testing  // record total cycle done

            // for Read_RDT_LOG
            UINT32  total_rdt_result_cnt;        // 4 bytes, total RDT recorde count
                                                // 64 bytes(accumulated)
            
            UINT16  first_weru_no;
            UINT16  first_page_idx;
            
            UINT16  last_weru_no;
            UINT16  last_page_idx;                 // 72 (A)
            
            UINT32  SLC_full_start_time;          // 4 Byte. unit: 100 millisecond
            UINT32  SLC_full_end_time;            // 4 Byte. unit: 100 millisecond
            UINT32  TLC_full_start_time;          // 4 Byte. unit: 100 millisecond
            UINT32  TLC_full_end_time;            // 4 Byte. unit: 100 millisecond
            
            UINT32  SLC_sampling_start_time;      // 4 Byte. unit: 100 millisecond
            UINT32  SLC_sampling_end_time;        // 4 Byte. unit: 100 millisecond
            UINT32  TLC_sampling_start_time;      // 4 Byte. unit: 100 millisecond
            UINT32  TLC_sampling_end_time;        // 4 Byte. unit: 100 millisecond
            
            UINT32  ODT_Log:1;                    // 4 Byte.
            UINT32  last_record_cnt:31;           // 4 Byte. record RDT last result.
            
            UINT32  rdt_result_pn;                // 4 bytes @ 0x6C, RDT result part number of a cycle.
            UINT32  cycle_rec_idx;                // (116)
            UINT32  last_cycle_rec_in_rdt;
            UINT16  stage;                        // preload data
            UINT16  RDT_FW_Flag;                  // MP can't use it (for FW)
            UINT16  pwr_cycle_cnt;                // 128 no more
            BYTE    rev[2];
        } h;
        BYTE rev[128];
    } head;
    RDT_BLOCK_RESULT Blk_Result[MAX_RESULT_COUNT];
} RDT_RESULT;


typedef struct _CYCLE_RESULT 
{
    union
    {
        struct
        {
            UINT32  signature;                   // 4 Byte  byte#0/#1 record TF, SF, SS, TS or CY
            UINT32  cycle;                       // 4 Byte, Result cycle
            UINT32  version;                     // 4 Byte
            //u8   rev[3];                        //
            UINT16  max_temp;                    // 2 Byte, MAX. temperature (C)
            UINT16  min_temp;                    // 2 Byte, MIN. temperature (C)
            UINT32  count;                       // 4 Byte
            //UINT32  start_time;                  // 4 Byte
            UINT32  RDT_FW_Use;                  // MP can't use it (for FW)
            UINT32  end_time;                    // 4 Byte
            //u32  last_mu_off;
            //u32  rdt_rec_indx;

            // time stamp of TLC Full Did
            UINT32  tlc_full_die_cycle;          // 4 Bytes, total cycle of TLC Full Die Testing
            UINT32  tlc_full_die_timestamp;      // 4 Bytes, Time Stamp of TLC Full Die Testing  // record total cycle done

            // time stamp of SLC Full Did
            UINT32  slc_full_die_cycle;          // 4 Bytes, total cycle of SLC Full Die Testing
            UINT32  slc_full_die_timestamp;      // 4 Bytes, Time Stamp of SLC Full Die Testing  // record total cycle done

            // time stamp of TLC Sampling Did
            UINT32  tlc_sampling_cycle;          // 4 Bytes, total cycle of TLC Sampling Testing
            UINT32  tlc_sampling_timestamp;      // 4 Bytes, Time Stamp of TLC Sampling Testing  // record total cycle done

            // time stamp of SLC Sampling Did
            UINT32  slc_sampling_cycle;          // 4 Bytes, total cycle of SLC Sampling Testing
            UINT32  slc_sampling_timestamp;      // 4 Bytes, Time Stamp of SLC Sampling Testing  // record total cycle done

            // for Read_RDT_LOG
            UINT32  total_rdt_result_cnt;        // 4 bytes, total RDT recorde count
                                                // 64 bytes(accumulated)
            
            UINT16  first_weru_no;
            UINT16  first_page_idx;
            
            UINT16  last_weru_no;
            UINT16  last_page_idx;                 // 72 (A)
            
            UINT32  SLC_full_start_time;          // 4 Byte. unit: 100 millisecond
            UINT32  SLC_full_end_time;            // 4 Byte. unit: 100 millisecond
            UINT32  TLC_full_start_time;          // 4 Byte. unit: 100 millisecond
            UINT32  TLC_full_end_time;            // 4 Byte. unit: 100 millisecond
            
            UINT32  SLC_sampling_start_time;      // 4 Byte. unit: 100 millisecond
            UINT32  SLC_sampling_end_time;        // 4 Byte. unit: 100 millisecond
            UINT32  TLC_sampling_start_time;      // 4 Byte. unit: 100 millisecond
            UINT32  TLC_sampling_end_time;        // 4 Byte. unit: 100 millisecond
            
            UINT16  ODT_Log;                      // 2 Byte.
            UINT16  GPIO_detect;                  // 2 Byte. record each channel is over current or not.
            
            UINT32  rdt_result_pn;                // 4 bytes @ 0x6C, RDT result part number of a cycle.
            UINT32  cycle_rec_idx;                // (116)
            UINT32  last_cycle_rec_in_rdt;
            UINT16  stage;                        // preload data
            UINT16  RDT_FW_Flag;                  // MP can't use it (for FW)
            UINT16  pwr_cycle_cnt;                // 128 no more
            BYTE    rev[2];
        } h;
        BYTE rev[128];
    } head;
    RDT_C_RESULT C_Result[MAX_RESULT_COUNT];
} CYCLE_RESULT;


typedef struct _BURNER_START_OPTION
{
    union {
        struct _BIT{
            BYTE  Rsv_1: 1;                   // bit#0
            BYTE  Rsv_2: 1;                   // bit#1
            BYTE  Rsv_3: 1;                   // bit#2
            BYTE  Rsv_4: 1;                   // bit#3
            BYTE  Force_EraseAll: 1;          // bit#4
            BYTE  Wait_Trigger: 1;            // bit#5
            BYTE  Skip_Scan_Later_Defect: 1;  // bit#6
            BYTE  Rsv_5: 1;                   // bit#7
        } Bit;
        BYTE Option;
    } Start_Option;
} BURNER_START_OPTION, *PBURNER_START_OPTION;
 
 
typedef struct _sss_blk_info_status_t
{
    BYTE  fe_info_status;
    BYTE  fcty_dft_status;
    BYTE  blk_dft_status;
    BYTE  ec_tbl_status;
    BYTE  phy_cal_status;
    BYTE  misc_ctrl_status;
    BYTE  config_tbl_status;
    BYTE  mr_phypatch_status;
    BYTE  rsv[120];
} sss_blk_info_status_t;


typedef struct _sss_WPB0_status_t
{
    sss_blk_info_status_t  Blk_Info_flag;
    UINT16  defect_blocks_per_plane_threshold;
    UINT16  all_good_block_WERUs_threshold;
    UINT16  rsv[2];
    UINT16  defect_blocks_per_plane[32];
    UINT16  all_good_block_WERUs;
    BYTE    rsv1[310];
} sss_wpb0_status_t;


// log event
typedef struct _sys_log_info_t
{
    BYTE  cur_block_num;
    BYTE  block_cnt;
    BYTE  min_block_num;
    BYTE  max_block_num;
    BYTE  rsv[508];
} sys_log_info_t;


// parse defect
typedef union _udev_addr_t
{
    struct
    {
        BYTE  ch;
        BYTE  target;
        BYTE  lun;
        BYTE  plane;
    } u;
    UINT32 v;
} udev_addr_t;


// burner start option from binary 0x10 ~ 0x1F
typedef union sss_startup_option_sub_flag_t_1
{
    struct {
        //LSB
        UINT32 SSS_burner_Report_FLASH_ID_MODE : 1;
        UINT32 SSS_burner_ADATA_Soritng_MARK_BAD_Recovery : 1;
        //To avoid ADATA Flash not clean issue, we change the DF_MARK scan rule
        //Only scan DF_MARK at Col 0,16K with SLC mode,
        //which will be judged to bad block when anyone of those bytes is 0x00
        UINT32 SSS_burner_ADATA_Scan_DF_MARK_with_0x00 : 1;
        UINT32 rsv : 29;
        //MSB
    } option1;
    UINT32 v;
} SSS_Startup_Option_Flag_1;


typedef union _sss_startup_option_sub_flag_t_2
{
    struct {
        UINT32 rsv;
    } option2;
    UINT32 v;
} SSS_Startup_Option_Flag_2;


//
// overwrite_support_mode:
//     Toggle1: 0x40, TM: 0x40 ~ 0x44
//     Toggle2: 0x80, TM: 0x80 ~ 0x83
//     if set 0xFF, burner will use the highest speed that was defined in the burner support list
//
//     Mode Speed    Mode Speed
//     0x40 DDR64    0x80 DDR200
//     0x41 DDR80    0x81 DDR266
//     0x42 DDR100   0x82 DDR320
//     0x43 DDR160   0x83 DDR400
//     0x44 DDR200
//
typedef struct _sss_startup_option_t
{
    SSS_Startup_Option_Flag_1 startup_option1;
    UINT32 MP_inject_WPB0_DF;          //4~7
    BYTE  overwrite_support_mode;     //8
    //Move to Write_Confirguration_Tbl flow when MP ready
    BYTE  overwrite_scan_DF_MARK;     //9
    //Move to Write_Confirguration_Tbl flow when MP ready
    BYTE  overwrite_SMART_temperature;//10
    BYTE  wpb0_read_write_test_count; //11
    //Move to Write_Confirguration_Tbl flow when MP ready
    UINT16 overwrite_later_scan_DF;    //12~13
    UINT16 rsv1;                       //14~15
    UINT32 rsv2[4];                    //16~31
} SSS_Startup_Option;


//20161124 Karen Tsai
typedef struct _layout_hdr
{
    char    signature[8];
    UINT16  num_of_weru;
    BYTE    width_nominal;
    BYTE    reserved[5];
    UINT32  crc;
    UINT32  size;
} layout_hdr;

#pragma pack()

#endif  // __SSD_DEFINITION__


//+++++AES+++++++++++++++
#define CIPHER_SIGNATURE  "3system"

//-------------------------------------------------------------------------------------------------
BYTE g_AES_KEY[] = { 0x2c, 0x7f, 0x16, 0x17, 0x29, 0xaf, 0xd3, 0xa7, 0xac, 0xf8, 0x16, 0x89, 0x10, 0xd0, 0x50, 0x3d };

#pragma pack(1)

// 64 bytes
typedef struct _CIPHER_HEADER
{
	char    Signature[16];
	UINT32  Size;
	UINT32  CRC;
	BYTE    Rsv[40];
} CIPHER_HEADER, *PCIPHER_HEADER;

#pragma pack()
//+++++AES+++++++++++++++