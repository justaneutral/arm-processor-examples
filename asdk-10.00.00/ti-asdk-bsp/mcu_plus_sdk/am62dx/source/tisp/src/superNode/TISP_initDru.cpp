#include <TISP_initDru.hpp>

#if !defined(HOST_EMULATION) && __C7X_VEC_SIZE_BITS__ == 512
/*Configure CLEC*/
void TISP::appC7xClecInitDru(void)
{
   CSL_ClecEventConfig cfgClec;
   CSL_CLEC_EVTRegs   *clecBaseAddr = (CSL_CLEC_EVTRegs *) CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;
   uint32_t            i;
   uint32_t            dru_input_start = 192;
#if defined(SOC_J784S4)
   dru_input_start = DRU_LOCAL_EVENT_START_J784S4;
#else
   dru_input_start = DRU_LOCAL_EVENT_START_DEFAULT;
#endif
   uint32_t dru_input_num = 16;

   /*Only configuring 16 channels*/
   for (i = dru_input_start; i < (dru_input_start + dru_input_num); i++) {
      /* Configure CLEC */
      cfgClec.secureClaimEnable = FALSE;
      cfgClec.evtSendEnable     = TRUE;

      /* cfgClec.rtMap value is different for each C7x */
      cfgClec.rtMap = CSL_CLEC_RTMAP_CPU_4;

      cfgClec.extEvtNum = 0;
      cfgClec.c7xEvtNum = (i - dru_input_start) + 32;
      CSL_clecConfigEvent(clecBaseAddr, i, &cfgClec);
   }

   return;
}

int32_t TISP::test_sciclientDmscGetVersion(char *version_str, uint32_t version_str_size)
{
   int32_t retVal = 0;

   struct tisci_msg_version_req  request;
   const Sciclient_ReqPrm_t      reqPrm = {TISCI_MSG_VERSION, TISCI_MSG_FLAG_AOP, (uint8_t *) &request, sizeof(request),
                                           SCICLIENT_SERVICE_WAIT_FOREVER};
   struct tisci_msg_version_resp response;
   Sciclient_RespPrm_t           respPrm = {0, (uint8_t *) &response, sizeof(response)};

   retVal = Sciclient_service(&reqPrm, &respPrm);
   if (0 == retVal) {
      if (respPrm.flags == TISCI_MSG_FLAG_ACK) {
         if (version_str == NULL) {
            printf("SCICLIENT: DMSC FW version [%s]\n", (char *) response.str);
            printf("SCICLIENT: DMSC FW revision 0x%x  \n", response.version);
            printf("SCICLIENT: DMSC FW ABI revision %d.%d\n", response.abi_major, response.abi_minor);
         }
         else {
            snprintf(version_str, version_str_size, "version %s, revision 0x%x, ABI %d.%d", (char *) response.str,
                     response.version, response.abi_major, response.abi_minor);
         }
      }
      else {
         retVal = -1;
      }
   }
   if (retVal != 0) {
      printf("SCICLIENT: ERROR: DMSC Firmware Get Version failed !!!\n");
   }

   return (retVal);
}

#endif
