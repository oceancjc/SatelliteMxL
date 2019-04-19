/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "devip.h"
#include "Sate_func.h"
#include "Net_Conf.h"
/** Initializes the devip module */
int modifyipaddr = 0;

void
init_devip(void)
{
    const oid ipaddr_oid[] = { 1,3,6,1,4,1,71200,1,1 };
    const oid submask_oid[] = { 1,3,6,1,4,1,71200,1,2 };
    const oid gateway_oid[] = { 1,3,6,1,4,1,71200,1,3 };
    const oid apply_oid[] = { 1,3,6,1,4,1,71200,1,4 };
    const oid devmode_oid[] = { 1,3,6,1,4,1,71200,1,5 };
    const oid sysreset_oid[] = {1,3,6,1,4,1,71200,1,6 };
  DEBUGMSGTL(("devip", "Initializing\n"));

    netsnmp_register_scalar(
        netsnmp_create_handler_registration("ipaddr", handle_ipaddr,
                               ipaddr_oid, OID_LENGTH(ipaddr_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("submask", handle_submask,
                               submask_oid, OID_LENGTH(submask_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("gateway", handle_gateway,
                               gateway_oid, OID_LENGTH(gateway_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("apply", handle_apply,
                               apply_oid, OID_LENGTH(apply_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("devmode", handle_devmode,
                               devmode_oid, OID_LENGTH(devmode_oid),
                               HANDLER_CAN_RWRITE
        ));
    netsnmp_register_scalar(
        netsnmp_create_handler_registration("sysreset", handle_sysreset,
                               sysreset_oid, OID_LENGTH(sysreset_oid),
                               HANDLER_CAN_RWRITE
                               
        ));
}

int
handle_ipaddr(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                                     /* XXX: a pointer to the scalar's data */ipaddr,
                                     /* XXX: the length of the data in bytes */strlen(ipaddr));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            // ret = netsnmp_check_vb_type(requests->requestvb, ASN_OCTET_STR);
            // if ( ret != SNMP_ERR_NOERROR ) {
                // netsnmp_set_request_error(reqinfo, requests, ret );
            // }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            // if (/* XXX if malloc, or whatever, failed: */) {
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            // }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            /* XXX: perform the value change here */
            strcpy(ipaddr,requests->requestvb->val.string);
            modifyipaddr = 1;
            if (/* XXX: error? */strlen(ipaddr) == 0) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE/* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            // }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            // }
            break;

        default:
            /* we should never get here, so this is a really bad error */
             snmp_log(LOG_ERR, "unknown mode (%d) in handle_ipaddr\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_submask(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                                     /* XXX: a pointer to the scalar's data */maskaddr,
                                     /* XXX: the length of the data in bytes */strlen(maskaddr));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            // ret = netsnmp_check_vb_type(requests->requestvb, ASN_OCTET_STR);
            // if ( ret != SNMP_ERR_NOERROR ) {
                // netsnmp_set_request_error(reqinfo, requests, ret );
            // }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            // if (/* XXX if malloc, or whatever, failed: */) {
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            // }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            /* XXX: perform the value change here */
            strcpy(maskaddr,requests->requestvb->val.string);
            modifyipaddr = 1;
            if (/* XXX: error? */strlen(maskaddr) == 0) {
                modifyipaddr = 0;
                netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_RESOURCEUNAVAILABLE /* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            // }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            // }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_submask\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_gateway(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                                     /* XXX: a pointer to the scalar's data */getwayaddr,
                                     /* XXX: the length of the data in bytes */strlen(getwayaddr));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            // ret = netsnmp_check_vb_type(requests->requestvb, ASN_OCTET_STR);
            // if ( ret != SNMP_ERR_NOERROR ) {
                // netsnmp_set_request_error(reqinfo, requests, ret );
            // }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            // if (/* XXX if malloc, or whatever, failed: */) {
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            // }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            /* XXX: perform the value change here */
            strcpy(getwayaddr,requests->requestvb->val.string);
            modifyipaddr = 1;
            if (/* XXX: error? */strlen(getwayaddr) == 0) {
                modifyipaddr = 0;
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE/* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            // }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            // }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_gateway\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_apply(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     /* XXX: a pointer to the scalar's data */&modifyipaddr,
                                     /* XXX: the length of the data in bytes */sizeof(modifyipaddr));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            // ret = netsnmp_check_vb_type(requests->requestvb, ASN_OCTET_STR);
            // if ( ret != SNMP_ERR_NOERROR ) {
                // netsnmp_set_request_error(reqinfo, requests, ret );
            // }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            // if (/* XXX if malloc, or whatever, failed: */) {
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            // }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            /* XXX: perform the value change here */
            if (modifyipaddr == 1) {
                int statusnum = 0;
                statusnum = setIfAddr( ipaddr, maskaddr, getwayaddr );
                if( statusnum < 0)    printf("Change Ip address Error, Error code is: %d\n",statusnum);
                modifyipaddr = 0;
            }
            else{
                printf("Nothing updated!\n");
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            // }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            // }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_apply\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}
int
handle_devmode(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     /* XXX: a pointer to the scalar's data */&SW_MODE,
                                     /* XXX: the length of the data in bytes */sizeof(SW_MODE));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            // ret = netsnmp_check_vb_type(requests->requestvb, ASN_INTEGER);
            // if ( ret != SNMP_ERR_NOERROR ) {
                // netsnmp_set_request_error(reqinfo, requests, ret );
            // }
            break;

        case MODE_SET_RESERVE2:
            // /* XXX malloc "undo" storage buffer */
            // if (/* XXX if malloc, or whatever, failed: */) {
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            // }
            // break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            /* XXX: perform the value change here */
            
            if (SW_MODE != *requests->requestvb->val.integer) {
                printf("Work mode changes to %d\n",(int)(*requests->requestvb->val.integer));
            }
            SW_MODE = *requests->requestvb->val.integer;
            udpTarget_Modeupdate(SW_MODE); 
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            // }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            // }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_devmode\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

int
handle_sysreset(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret,i=0;
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    switch(reqinfo->mode) {

        case MODE_GET:
     snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     /* XXX: a pointer to the scalar's data */&i,
                                     /* XXX: the length of the data in bytes */sizeof(i));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            // ret = netsnmp_check_vb_type(requests->requestvb, ASN_INTEGER);
            // if ( ret != SNMP_ERR_NOERROR ) {
                // netsnmp_set_request_error(reqinfo, requests, ret );
            // }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            // if (/* XXX if malloc, or whatever, failed: */) {
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            // }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            /* XXX: perform the value change here */
            //if (/* XXX: error? */0) {
             //   netsnmp_set_request_error(reqinfo, requests, /* some error */);
           // }
        if(1 == *requests->requestvb->val.integer)    system("shutdown -h now");
        else    system("reboot");
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            // }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            // if (/* XXX: error? */) {
                // /* try _really_really_ hard to never get to this point */
                // netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            // }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_sysreset\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}