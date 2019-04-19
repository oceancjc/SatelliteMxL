/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "trap.h"
#include <string.h>
#include "LoopApp.h"

extern const oid snmptrap_oid[];
extern const size_t snmptrap_oid_len;

int
send_reset_trap( char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid reset_oid[] = { 1,3,6,1,4,1,71200,98,3 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        reset_oid, sizeof(reset_oid));
    
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_start_trap( char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid start_oid[] = { 1,3,6,1,4,1,71200,98,4 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        start_oid, sizeof(start_oid));
    
    /*
     * Add any objects from the trap definition
     */
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_rxpwrtrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid rxpwrtrap_oid[] = { 1,3,6,1,4,1,71200,98,5 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, /* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, /* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, /* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4, /* insert index here */ };
    const oid rxpwr_oid[] = { 1,3,6,1,4,1,71200,2,1,20, /* insert index here */ };
    const oid rxpwrlow_oid[] = { 1,3,6,1,4,1,71200,4,1,3, /* insert index here */ };
    const oid rxpwrhigh_oid[] = { 1,3,6,1,4,1,71200,4,1,4, /* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        rxpwrtrap_oid, sizeof(rxpwrtrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        &dev->sdPtr[i]->frequencyInKHz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        rxpwr_oid, OID_LENGTH(rxpwr_oid),
        ASN_INTEGER,
        /* Set an appropriate value for rxpwr */
        &dev->sdPtr[i]->pwr_100dbm, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        rxpwrlow_oid, OID_LENGTH(rxpwrlow_oid),
        ASN_INTEGER,
        /* Set an appropriate value for rxpwrlow */
        &dev->sd_thPtr[i]->rxpwrLow, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        rxpwrhigh_oid, OID_LENGTH(rxpwrhigh_oid),
        ASN_INTEGER,
        /* Set an appropriate value for rxpwrhigh */
        &dev->sd_thPtr[i]->rxpwrHigh, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_snrtrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid snrtrap_oid[] = { 1,3,6,1,4,1,71200,98,6 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, i/* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, i/* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, i/* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4,i /* insert index here */ };
    const oid snr_oid[] = { 1,3,6,1,4,1,71200,2,1,21,i /* insert index here */ };
    const oid snrlow_oid[] = { 1,3,6,1,4,1,71200,4,1,5, i/* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        snrtrap_oid, sizeof(snrtrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        &dev->sdPtr[i]->frequencyInKHz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        snr_oid, OID_LENGTH(snr_oid),
        ASN_INTEGER,
        /* Set an appropriate value for snr */
        &dev->sdPtr[i]->snr, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        snrlow_oid, OID_LENGTH(snrlow_oid),
        ASN_INTEGER,
        /* Set an appropriate value for snrlow */
        &dev->sd_thPtr[i]->snrLow, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_mertrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid mertrap_oid[] = { 1,3,6,1,4,1,71200,98,7 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, /* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, /* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, /* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4, /* insert index here */ };
    const oid mer_oid[] = { 1,3,6,1,4,1,71200,2,1,22, /* insert index here */ };
    const oid merlow_oid[] = { 1,3,6,1,4,1,71200,4,1,6, /* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        mertrap_oid, sizeof(mertrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mer_oid, OID_LENGTH(mer_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mer */
        &dev->sdPtr[i]->mer, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        merlow_oid, OID_LENGTH(merlow_oid),
        ASN_INTEGER,
        /* Set an appropriate value for merlow */
        &dev->sd_thPtr[i]->merLow, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_evmtrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid evmtrap_oid[] = { 1,3,6,1,4,1,71200,98,8 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, i/* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, i/* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, i/* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4, i/* insert index here */ };
    const oid evm_oid[] = { 1,3,6,1,4,1,71200,2,1,23,i /* insert index here */ };
    const oid evmlow_oid[] = { 1,3,6,1,4,1,71200,4,1,7,i /* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        evmtrap_oid, sizeof(evmtrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        &dev->sdPtr[i]->frequencyInKHz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        evm_oid, OID_LENGTH(evm_oid),
        ASN_INTEGER,
        /* Set an appropriate value for evm */
        &dev->sdPtr[i]->evm, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        evmlow_oid, OID_LENGTH(evmlow_oid),
        ASN_INTEGER,
        /* Set an appropriate value for evmlow */
        &dev->sd_thPtr[i]->evmLow, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_freqoffsettrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid freqoffsettrap_oid[] = { 1,3,6,1,4,1,71200,98,9 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, i/* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, i/* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, i/* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4, i/* insert index here */ };
    const oid freqoffset_oid[] = { 1,3,6,1,4,1,71200,2,1,25, i/* insert index here */ };
    const oid freqoffsetmax_oid[] = { 1,3,6,1,4,1,71200,4,1,8, i/* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        freqoffsettrap_oid, sizeof(freqoffsettrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        &dev->sdPtr[i]->frequencyInKHz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        freqoffset_oid, OID_LENGTH(freqoffset_oid),
        ASN_INTEGER,
        /* Set an appropriate value for freqoffset */
        &dev->sdPtr[i]->cfo_Hz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        freqoffsetmax_oid, OID_LENGTH(freqoffsetmax_oid),
        ASN_INTEGER,
        /* Set an appropriate value for freqoffsetmax */
        &dev->sd_thPtr[i]->cfoMax_Hz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_symoffsettrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid symoffsettrap_oid[] = { 1,3,6,1,4,1,71200,98,10 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, i/* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, i/* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, i/* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4,i /* insert index here */ };
    const oid symoffset_oid[] = { 1,3,6,1,4,1,71200,2,1,26, i/* insert index here */ };
    const oid symoffsetmax_oid[] = { 1,3,6,1,4,1,71200,4,1,9, i/* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        symoffsettrap_oid, sizeof(symoffsettrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        &dev->sdPtr[i]->frequencyInKHz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        symoffset_oid, OID_LENGTH(symoffset_oid),
        ASN_INTEGER,
        /* Set an appropriate value for symoffset */
        &dev->sdPtr[i]->fo_symbol, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        symoffsetmax_oid, OID_LENGTH(symoffsetmax_oid),
        ASN_INTEGER,
        /* Set an appropriate value for symoffsetmax */
        &dev->sd_thPtr[i]->sfo_symbol, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_agclocktrap_trap( void )
{
    // netsnmp_variable_list  *var_list = NULL;
    // const oid agclocktrap_oid[] = { 1,3,6,1,4,1,71200,98,11 };
    // const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, /* insert index here */ };
    // const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3, /* insert index here */ };
    // const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, /* insert index here */ };
    // const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4, /* insert index here */ };
    // const oid agclock_oid[] = { 1,3,6,1,4,1,71200,2,1,27, /* insert index here */ };
    // const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    // const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    // /*
     // * Set the snmpTrapOid.0 value
     // */
    // snmp_varlist_add_variable(&var_list,
        // snmptrap_oid, snmptrap_oid_len,
        // ASN_OBJECT_ID,
        // agclocktrap_oid, sizeof(agclocktrap_oid));
    
    // /*
     // * Add any objects from the trap definition
     // */
    // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    // snmp_varlist_add_variable(&var_list,
        // mode_oid, OID_LENGTH(mode_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for mode */
        // NULL, 0);
    // snmp_varlist_add_variable(&var_list,
        // modulation_oid, OID_LENGTH(modulation_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for modulation */
        // NULL, 0);
    // snmp_varlist_add_variable(&var_list,
        // frequency_oid, OID_LENGTH(frequency_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for frequency */
        // NULL, 0);
    // snmp_varlist_add_variable(&var_list,
        // agclock_oid, OID_LENGTH(agclock_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for agclock */
        // NULL, 0);
    // snmp_varlist_add_variable(&var_list,
        // status_oid, OID_LENGTH(status_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for status */
        // NULL, 0);
    // snmp_varlist_add_variable(&var_list,
        // stringdesc_oid, OID_LENGTH(stringdesc_oid),
        // ASN_OCTET_STR,
        // /* Set an appropriate value for stringdesc */
        // NULL, 0);

    // /*
     // * Add any extra (optional) objects here
     // */

    // /*
     // * Send the trap to the list of configured destinations
     // *  and clean up
     // */
    // send_v2trap( var_list );
    // snmp_free_varbind( var_list );

    // return SNMP_ERR_NOERROR;
}
int
send_feclocktrap_trap( int i, char* s )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid feclocktrap_oid[] = { 1,3,6,1,4,1,71200,98,12 };
    const oid inputport_oid[] = { 1,3,6,1,4,1,71200,2,1,2, i/* insert index here */ };
    const oid mode_oid[] = { 1,3,6,1,4,1,71200,2,1,3,i /* insert index here */ };
    const oid modulation_oid[] = { 1,3,6,1,4,1,71200,2,1,5, i/* insert index here */ };
    const oid frequency_oid[] = { 1,3,6,1,4,1,71200,2,1,4,i /* insert index here */ };
    const oid feclock_oid[] = { 1,3,6,1,4,1,71200,2,1,28, i/* insert index here */ };
    const oid status_oid[] = { 1,3,6,1,4,1,71200,98,1, 0 };
    const oid stringdesc_oid[] = { 1,3,6,1,4,1,71200,98,2, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        feclocktrap_oid, sizeof(feclocktrap_oid));
    
    /*
     * Add any objects from the trap definition
     */
      // snmp_varlist_add_variable(&var_list,
        // inputport_oid, OID_LENGTH(inputport_oid),
        // ASN_INTEGER,
        // /* Set an appropriate value for inputport */
        // NULL, 0);
    snmp_varlist_add_variable(&var_list,
        mode_oid, OID_LENGTH(mode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for mode */
        &dev->sdPtr[i]->standardMask, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        modulation_oid, OID_LENGTH(modulation_oid),
        ASN_INTEGER,
        /* Set an appropriate value for modulation */
        &dev->sdPtr[i]->modulation, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        frequency_oid, OID_LENGTH(frequency_oid),
        ASN_INTEGER,
        /* Set an appropriate value for frequency */
        &dev->sdPtr[i]->frequencyInKHz, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        feclock_oid, OID_LENGTH(feclock_oid),
        ASN_INTEGER,
        /* Set an appropriate value for feclock */
        &dev->sdPtr[i]->fecLock, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        status_oid, OID_LENGTH(status_oid),
        ASN_INTEGER,
        /* Set an appropriate value for status */
        &trap_status, sizeof(int));
    snmp_varlist_add_variable(&var_list,
        stringdesc_oid, OID_LENGTH(stringdesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for stringdesc */
        s, strlen(s));

    /*
     * Add any extra (optional) objects here
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
