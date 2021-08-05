/*!
 * @file convCode27.cpp
 * @author Arash Yazdani
 * @date July 21, 2021
 *
 * @details 
 *
 * @copyright 
 *
 * @license
 * This software may not be modified or distributed in any form, except as described in the LICENSE file.
 */

#include "convCode27.hpp"
#include "pdu.hpp"
#include "viterbi.h"


namespace ex2 {
  namespace sdr {

    convCode27::~convCode27() {
      //rate = 1/2;
      //constraint_length = 7;
    }

    PPDU_u8
    convCode27::encode(PPDU_u8 &payload) {
      
      //encodedPayload.resize((1/rate) * sizeof(payload)); //what if not integer?
      // hardcoded for k = 7 and rate = 1/2
      std::vector<uint8_t> g1 = {0, 1, 2, 3, 6};
      std::vector<uint8_t> g2 = {0, 2, 3, 5, 6};
      
      PPDU_u8::payload_t PayloadData = payload.getPayload();
      PayloadData.resize((1/rate) * sizeof(payload));
      // append 0 at beginning for k-1
      PPDU_u8::payload_t AppendedPayloadData (constraint_length - 1, 0);
      AppendedPayloadData.insert(AppendedPayloadData.end(), PayloadData.begin(), PayloadData.end());
      
      PPDU_u8::payload_t encodedPayloadData;
      // for loop of encoding 
      // hardcoded for rate = 1/2
      for(uint8_t i = 0; i<payload.payloadLength(); i++){
        encodedPayloadData[i] = adder(AppendedPayloadData[i + constraint_length - 1],g1);
        encodedPayloadData[i + 1] = adder(AppendedPayloadData[i + constraint_length - 1],g2);
      }
      PPDU_u8 encodedPayload(encodedPayloadData);
      
      return encodedPayload;
      
    }

    uint32_t
    convCode27::decode(const PPDU_u8::payload_t& encodedPayload, float snrEstimate,
      PPDU_u8::payload_t& decodedPayload) {

      (void) snrEstimate; // Not used in this mmethod

      decodedPayload.resize(0); // Resize in all FEC decode methods

      /* Init Vitrbi */
      void *vp;
      int framebits = 2048; // hardcoded for 27
      if((vp = create_viterbi27(framebits)) == NULL){
        // Init failed.
      }

      // TODO: Use snrEstimate here.

      /* Decode it and make sure we get the right answer */
      /* Initialize Viterbi decoder */
      init_viterbi27(vp,0);
      
      /* Decode block */
      uint8_t encodedArr[framebits+constraint_length-1];
      std::copy(encodedPayload.begin(), encodedPayload.end(), encodedArr);
      //uint8_t * encodedPtr = &encodedPayload[0];
      update_viterbi27_blk(vp,encodedArr,framebits+constraint_length-1);
      
      /* Do Viterbi chainback */
      //uint8_t * decodedPtr = &decodedPayload[0];
      uint8_t decodedArr[framebits];
      std::copy(decodedPayload.begin(), decodedPayload.end(), decodedArr);
      chainback_viterbi27(vp,decodedArr,framebits,0);

      return 0;
    }

    uint8_t
    adder(uint8_t * payload_sym , std::vector<uint8_t> g){
        uint8_t sum = * payload_sym;

        for (int i =0; i < g.size(); i++){
          //if (g[i] < constraint_length && g[i]>0) {
            sum += *(payload_sym - g[i]);
          //}
          // else throw sth?
        }
        // Should it necessarily be modulo-2? PDU looks like general
        return sum;

    }


  } /* namespace sdr */
} /* namespace ex2 */