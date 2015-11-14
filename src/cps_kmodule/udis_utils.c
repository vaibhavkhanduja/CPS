/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "udis_utils.h"

uint64_t ud_get_stub_size(void *entry){
	ud_t ud;

	ud_init(&ud);
	ud_set_mode(&ud, BITS_PER_LONG);
	ud_set_vendor(&ud, UD_VENDOR_ANY);
	ud_set_input_buffer(&ud, entry, 512);

	while(ud_disassemble(&ud)){
		if(ud.mnemonic == UD_Iret){
			return ud_insn_off(&ud) + ud_insn_len(&ud);
		}
	}

	return 0;
}

void ud_patch_addr(void *entry, void *addr){
	ud_t ud;
	char *match;
	void *patch_addr;
	int offset, ptr_size;

	ptr_size = sizeof(void *);

#ifdef CONFIG_X86_32
	match = "\x10\x10\x10\x10";
#elif defined(CONFIG_X86_64)
	match = "\x10\x10\x10\x10\x10\x10\x10\x10";
#endif

	ud_init(&ud);
	ud_set_mode(&ud, BITS_PER_LONG);
	ud_set_vendor(&ud, UD_VENDOR_ANY);
	ud_set_input_buffer(&ud, entry, ud_get_stub_size(entry));

	while(ud_disassemble(&ud)){
		if(ud.mnemonic == UD_Imov && (ud.operand[1].type == UD_OP_IMM || ud.operand[1].type == UD_OP_MEM)){
			offset = ud_insn_len(&ud) - ptr_size;
			patch_addr = entry + ud_insn_off(&ud) + offset;

			if(offset > 0 && !memcmp(patch_addr, match, ptr_size)){
				memcpy(patch_addr, &addr, ptr_size);
				return;
			}
		}
	}
}

void ud_patch_cmp(void *entry){
	int i;
	ud_t ud;
	void *patch_addr;

	addrs_t patch_addrs;
	patch_addrs.addr1 = NULL;
	patch_addrs.len1 = 0;
	patch_addrs.addr2 = NULL;
	patch_addrs.len2 = 0;

	ud_init(&ud);
	ud_set_mode(&ud, BITS_PER_LONG);
	ud_set_vendor(&ud, UD_VENDOR_ANY);
	ud_set_input_buffer(&ud, entry, ud_get_stub_size(entry));

	while(ud_disassemble(&ud)){

		if(ud.mnemonic != UD_Ijnz){
			//We're at the very first instruction
			if(patch_addrs.addr2 == NULL){
				patch_addrs.addr2 = entry + ud_insn_off(&ud);
				patch_addrs.len2 = ud_insn_len(&ud);

			//We're at any other instruction
			}else{
				patch_addrs.addr1 = patch_addrs.addr2;
				patch_addrs.len1 = patch_addrs.len2;

				patch_addrs.addr2 = entry + ud_insn_off(&ud);
				patch_addrs.len2 = ud_insn_len(&ud);
			}
		}

		if(ud.mnemonic == UD_Ijnz){ //same as jne
			//patch addr1 with NOPs
			for(i = 0; i < patch_addrs.len1; i++){
				//90 is NOP instruction
				memcpy(patch_addrs.addr1 + i, "\x90", 1);
			}

			//patch addr2 with NOPs
			for(i = 0; i < patch_addrs.len2; i++){
				//90 is NOP instruction
				memcpy(patch_addrs.addr2 + i, "\x90", 1);
			}

			//patch current addr with JMP
			patch_addr = entry + ud_insn_off(&ud);
			//EB is JMP instruction
			memcpy(patch_addr, "\xEB", 1);
		}
	}
}

void *ud_find_syscall_table_addr(void *entry){
	ud_t ud;

	ud_init(&ud);
	ud_set_mode(&ud, BITS_PER_LONG);
	ud_set_vendor(&ud, UD_VENDOR_ANY);
	ud_set_input_buffer(&ud, entry, 512);

	while(ud_disassemble(&ud)){
		if(ud.mnemonic == UD_Icall && ud_insn_len(&ud) == 7){
#ifdef CONFIG_X86_32
			return (void *)ud.operand[0].lval.udword;
#elif defined(CONFIG_X86_64)
			return (void *)(0xffffffff00000000 | ud.operand[0].lval.udword);
#endif
		}
	}

	return NULL;
}