/*
    WIP SDM APP
 TODO:
    Print the device certificate chain with the signer cert
    Populate IotConnectSdmConfig
        Keystore ID is based on ECC608 serial obtained with atca functions
        Alias is hardcoded to "Cert-0"? #defined in iotconnect_sdm.h?
    Implement IotConnectSdmSign by calling atca functions
    Get the token by calling
    Get device info by calling iotc_sdm_get_info
    Set up IoTConnect IotConnectClientConfig based on info obtained by iotc_sdm_get_info
*/

