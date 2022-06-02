//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//


#ifndef TO_AUTH_DRIVER_H
#define TO_AUTH_DRIVER_H

#ifdef __cplusplus
extern   "C" {
#endif


struct to_driver_parameters {
	// The CSR slot that will be used to obtain operational certificate
    // This slot number cannot be 0 at the time of calling get_certificate()
	// Note that this slot is NOT used for store_operational_certificate
    int operational_identity_slot;

    // This slot number should match the future operational slot of the certificate that is being requested by the CSR.
    // It should match the operational slot if you intend to overwrite the existing operational certificate with the new one.
    // It should not match the operational slot if you intend to roll certificates.
    // In case of rolling certificates make sure to account for that in the application
    // This slot number cannot be 0 at the time of calling generate_csr() or store_operational_certificate()
    int operational_csr_slot;

    // optional (default 0) - the slot where the bootstrap identity is stored and it is 0 by default in standard scenario
    int bootstrap_identity_slot;
};

int to_create_auth_driver( //
		IotcAuthInterface* driver_interface, //
		IotcDdimInterface* ddim_interface, // Optional DDIM interface that can be used to dynamically obtain device certificates from IoTConnect
		IotcAuthInterfaceContext* context, // This context will be returned and can be used to
		struct to_driver_parameters *driver_parameters //
		);
int to_release_auth_driver(IotcAuthInterfaceContext* context);


#ifdef __cplusplus
}
#endif

#endif // TO_AUTH_DRIVER_H
