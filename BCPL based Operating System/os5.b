import "io"

manifest
{ 
	pd_pdbr_ptr               = 0xC0000000,
	pd_free_page_list         = 0xC0400000, 
	pd_free_page_ptr          = 0xC0000800, 
	pd_intvec                 = 0xC0000810, 
	pd_info                   = 0xC0000830, 
	pd_pqueue                 = 0xC0000930, 
	pd_pqueue_first           = 0xC0000951,
	pd_pqueue_last            = 0xC0000952,
	pd_pqueue_len             = 0xC0000953,
	pd_pqueue_size            = 0xC0000954,
	pd_curr_proc              = 0xC0000955,
	pd_usr_stack              = 0x7FFFFFFF,
	pd_usr_code               = 0x00000000 //tried to do multi in one go, delaying until sunday now.
}

let get_page() be
{ 
	let r = pd_free_page_list ! (! pd_free_page_ptr);
	! pd_free_page_ptr +:= 1;
	resultis r; 
}

let get_free_page() be
{ 
	let page_addr = get_page();
	resultis page_addr bitor 1 
}

let get_page_table_num (addr) be
{ 
	resultis addr >> 22 //shift until pagenum is the only thing left
}

let get_page_num (addr) be
{
	resultis (addr >> 11) bitand 0x7FF //shift append virt
}

let get_wrd_num (addr) be
{ 
	resultis addr bitand 0x7FF 
}


let make_virt_addr(ptn, pn, wn) be
{ 
	let addr = ptn;
	addr := (addr << 11) + pn;
	addr := (addr << 11) + wn; 
	resultis addr; 
}

let get_virt_wrd(ptn, pn, offs) be
{ resultis ! make_virt_addr(ptn, pn, offs) }


manifest
{ 
	page_size_chars = 2048,
	page_size_bytes = 512,
	page_size_words = 128, 
	pd_pt_temp_page = 0xC09813FF, 
	pd_temp_page    = 0xC09FF800 
}

let load_program(fname, page_table) be
{ 
	let page_num = -1; 
	let buff_pos = page_size_chars;
	let page_addr;
	let r = devctl(DC_TAPE_LOAD, 1, fname, 'R');
	if r < 0 then
	{ 
		out("error %d for load '%s'\n", r, fname);
		finish 
	}
	while true do
	{ 
		if  buff_pos = page_size_chars then
		{ 
			buff_pos := 0;
			page_num +:= 1; 
			page_addr := get_page();
			(! pd_pt_temp_page) := page_addr bitor 1;
			! (page_table + page_num) := page_addr bitor 1 
		}
		r := devctl(DC_TAPE_READ, 1, pd_temp_page + buff_pos);
		if r < page_size_bytes then break;
		buff_pos +:= page_size_words 
	}
	(! pd_pt_temp_page) := 0; 
	devctl(DC_TAPE_UNLOAD, 1) 
}

manifest 
{
  pd_pt_new_page_dir         = 0xC0981702, 
  pd_new_page_dir            = 0xC0B81000, 
  pd_new_page_dir_user_code  = 0xC0B81000, 
  pd_new_page_dir_stack_code = 0xC0B811FF, 
  pd_pt_new_user_stack       = 0xC09815FF, 
  pd_new_user_stack          = 0xC0AFFFFF, 
  pd_pt_new_user_code        = 0xC0981400, 
  pd_new_user_code           = 0xC0A00000, 
  pd_page_dir                = 0xC0981000  
}

let new_pdbr_ptr(usr_program_name) be
{ 
	let page_dir_phy_addr, page_addr;
	page_dir_phy_addr := get_page();
	(! pd_pt_new_page_dir) := page_dir_phy_addr bitor 1;
	for ptn = 512 to 1023 do
	{ 
		pd_new_page_dir ! ptn := pd_page_dir ! ptn 
	}
	(! pd_pt_new_user_stack) := get_free_page();
	(! pd_new_user_stack) := get_free_page();
	(! pd_pt_new_user_code) := get_free_page();
	load_program(usr_program_name, pd_new_user_code);
	(! pd_new_page_dir_stack_code) := (! pd_pt_new_user_stack);
	(! pd_pt_new_user_stack) := 0; 
	(! pd_new_page_dir_user_code) := (! pd_pt_new_user_code);
	(! pd_pt_new_user_code) := 0; 
	(! pd_pt_new_page_dir) := 0;  
	resultis page_dir_phy_addr 
}

let pd_pqueue_add(pcb) be 
{ 
	if((! pd_pqueue_len)+1) = (! pd_pqueue_size) then  
	{ resultis nil }
  	if (! pd_pqueue_last ) = (! pd_pqueue_size) then
	{ (! pd_pqueue_last) := 0 }
    pd_pqueue ! (! pd_pqueue_last) := pcb;
    (! pd_pqueue_last) +:= 1 
}

let pd_pqueue_remove() be
{ 
	let ret_index = ! pd_pqueue_first;
	test (! pd_pqueue_first) + 1 = (! pd_pqueue_size) then
	{ 
		! pd_pqueue_first := 0 
	}
	else
	{ ! pd_pqueue_first +:= 1 
	} 
	resultis pd_pqueue ! ret_index 
} 

let setup_pqueue(pcb_ptr) be
{ 
	! pd_pqueue_first := 0;
	! pd_pqueue_last := 0;
	! pd_pqueue_len := 0;
	! pd_pqueue_size := 20;
	! pd_curr_proc := pcb_ptr 
} 

manifest
{ FLAG_R = 1, FLAG_Z = 2, FLAG_N = 4, FLAG_ERR = 8, FLAG_SYS = 16, FLAG_IP = 32, FLAG_VM = 64 }

manifest
{ PCB_FLAGS = 0, PCB_INTCODE = 1, PCB_INTADDR = 2, PCB_INTX = 3, PCB_PC = 4,
  PCB_FP = 5, PCB_SP = 6, PCB_R12 = 7, PCB_R11 = 8, PCB_R10 = 9, PCB_R9 = 10,
  PCB_R8 = 11, PCB_R7 = 12, PCB_R6 = 13, PCB_R5 = 14, PCB_R4 = 15,
  PCB_R3 = 16, PCB_R2 = 17, PCB_R1 = 18, PCB_R0 = 19, PCB_STATE = 20,
  PCB_PDBR_PHY_PTR = 21, 
  SIZEOF_PCB = 23 }

let make_pcb(pcb, pdbr_ptr_phy, pc, sp, code) be
{ 
	pcb ! PCB_R0 := 0;
	pcb ! PCB_R1 := 1*code;
	pcb ! PCB_R2 := 2*code;
	pcb ! PCB_R3 := 3*code;
	pcb ! PCB_R4 := 4*code;
	pcb ! PCB_R5 := 5*code;
	pcb ! PCB_R6 := 6*code;
	pcb ! PCB_R7 := 7*code;
	pcb ! PCB_R8 := 8*code;
	pcb ! PCB_R9 := 9*code;
	pcb ! PCB_R10 := 10*code;
	pcb ! PCB_R11 := 11*code;
	pcb ! PCB_R12 := 12*code;
	pcb ! PCB_SP := sp;
	pcb ! PCB_FP := sp;
	pcb ! PCB_PC := pc;
	pcb ! PCB_INTX := 0;
	pcb ! PCB_INTADDR := 0;
	pcb ! PCB_INTCODE := 0;
	pcb ! PCB_FLAGS := FLAG_R;
	pcb ! PCB_STATE := 'R';
	pcb ! PCB_PDBR_PHY_PTR := pdbr_ptr_phy 
}

let add_new_pcb(usr_program_name) be
{ 
	let new_process_pcb, new_process_pdbr_ptr;
	if (! pd_pqueue_len) + 1 >= (! pd_pqueue_size) then
	{ 
		outs("process queue full only 20 processes allowed");
		return 
	}
	new_process_pcb := newvec(SIZEOF_PCB);
	new_process_pdbr_ptr := new_pdbr_ptr(usr_program_name);
}

let setup_pocesses() be 
{ 
	let curr_pdbr_phy_addr;
	let curr_pcb_ptr = newvec(SIZEOF_PCB);
	assembly 
	{ 
		getsr r1, $PDBR 
		store r1, [<curr_pdbr_phy_addr>] 
	}
	make_pcb(curr_pcb_ptr, curr_pcb_ptr, pd_usr_code, pd_usr_stack, !pd_pqueue_len); 
	setup_pqueue(curr_pcb_ptr);
	! pd_curr_proc := curr_pcb_ptr 
}

manifest
{ iv_none = 0,        iv_memory = 1,      iv_pagefault = 2,   iv_unimpop = 3,
  iv_halt = 4,        iv_divzero = 5,     iv_unwrop = 6,      iv_timer = 7,
  iv_privop = 8,      iv_keybd = 9,       iv_badcall = 10,    iv_pagepriv = 11,
  iv_debug = 12,      iv_intrfault = 13 }

let page_fault_handler(a, addr, c) be
{ 
	let ptn  = get_page_table_num(addr); 
	let pn   = get_page_num(addr);       
	let wn   = get_wrd_num(addr);       
	let page_addr_loc = make_virt_addr(770, ptn, pn); 
	let npte = get_free_page();  
	out("SIGSEGV! address = %x, %x/%x/%x \n", addr, ptn, pn, wn);
	out("a = %x, address = %x, c = %x)\n", a, addr, c);
	outch('\n'); 
	! page_addr_loc := npte;  
	ireturn 
}

let halt_handler() be
{
	let os_stack_ptr = 0xBFFFFFC7;
	let os_start_ptr = 0x8000047B;
	outs("STOP! You've violated the law! (HALT)\n");
	assembly
	{ 
		load  r1, 1
		setfl r1, $ip
		setfl r1, $sys
		load  pc, 0x800004A2
	}
	finish; 
	ireturn 
}

let set_timer_handler(t) be
{ 
	assembly
	{ 
		load  r1, [<t>]
		setsr r1, $timer 
	} 
}
let timer_handler() be
{ 
	out("TIMER!\n");
	pd_pqueue_add(!pd_curr_proc);
	! pd_curr_proc := pd_pqueue_remove();
	set_timer_handler(50000000);
	ireturn 
}

let setup_interupts() be
{ 
	pd_intvec ! iv_pagefault := page_fault_handler;
	pd_intvec ! iv_timer     := timer_handler;
	pd_intvec ! iv_halt      := halt_handler 
}

let sc1(code, regn, regv, pc, fp, sp, r12, r11, r10, r9, r8, r7, r6, r5, r4, r3, r2, r1, r0) be
{ 
	out("sc1(%d) %d %d %d %d\n", code, r1, r2, r3, r4);  
	ireturn 
}

let sc2(code, regn, regv, pc, fp, sp) be
{ 
	let nargs = (fp ! 2) / 2;
	out("sc2(%d, %d, %d, %d)\n", fp!3, fp!4, fp!5, fp!6);
	ireturn
}

let setup_cgbase() be
{ 
	pd_info ! 1 := sc1;
	pd_info ! 2 := sc2; 
}

let print_mem_map() be
{ 
	let pdbr;
	assembly
	{ 
		getsr r1, $pdbr
		store r1, [<pdbr>] 
	}
	out("00000000 to FFFFFFFF: PDBR at p%x, v%x\n", pdbr, pd_pdbr_ptr);
	for ptn = 1023 to 0 by -1 do
	{ 
		let pde = get_virt_wrd(770,770, ptn); 
		let ptrange = ptn << 22;         
		if pde = 0 then loop;               
		out("   %08x to %08x: ", ptrange, ptrange + 0x3FFFFF);
		if (pde bitand 1) = 0 then          
		{ 
			out("%08x - invalid\n", pde);
			loop 
		}
		out("pp %d at %x\n", pde >> 11, pde >> 11 << 11);
		for pn = 2047 to 0 by -1 do
		{ 
			let pte = get_virt_wrd(770, ptn, pn);
			let range = ptrange + (pn << 11); 
			if pte = 0 then loop;           
			out("      %08x to %08x: ", range, range + 0x7FF);
			if (pte bitand 1) = 0 then        
			{ 
				out("%08x - invalid\n", pte);
				loop 
			}
			out("pp %d at %x\n", pte >> 11, pte >> 11 << 11) 
		} 
	} 
}

let inlast = '\n';

let newinput() be
{ 
	outs("> ");
	inlast := 0; 
}

let inchar() be
{ 
	if inlast <> 0 then
	{ 
		let r = inlast;
		inlast := 0;
		resultis r; 
	}
		
	resultis inch(); 
}

let inpart(s, max) be
{ 
	let c = inchar(), n = 0;
	while true do
	{ 
		if c = '\n' then
		{ 
			inlast := '\n';
			resultis nil; 
		}
		if c > ' ' then break;
			c := inchar(); 
	}
	while true do
	{ 
		if n < max then
			{ 
				byte n of s := c;
				n +:= 1 
			}
		c := inchar();
		if c <= ' ' then
		{ 
			inlast := c;
			break 
		} 
	}
	byte n of s := 0;
	resultis s 
}

let streq(a, b) be
{ 
	let i = 0;
	while true do
	{
		let ca = byte i of a, cb = byte i of b;
		if ca <> cb then resultis false;
		if ca = 0 then resultis true;
		i +:= 1 
	}
}

let user_interface() be
{ 
	let stack_ptr = 0x7FFFFFFF;
	while true do
	{ 
		let s = vec(30);
		newinput();
		s := inpart(s, 119);
		if s = nil then loop;
		if streq(s, "exit") then
			break;
		if streq(s, "run") then
		{
			set_timer_handler(50000000);
			assembly
			{ 
				load  r2, [<stack_ptr>]
				load  r1, 0
				setfl r1, $ip
				setfl r1, $sys
				store r2, sp
				store r1, fp
				jump  0 
			}
		}
		out("bad command '%s'\n", s) 
	} 
}

let make_heap() be 
{ 
	let begin = (((! 0x80000101) >> 10) + 1) << 10, end = ! 0x80000100;
	init(begin, end - begin); 
}
  
let start() be
{ 
	let heap = vec 47;
	let x = 12;
	let y = x*x;	
	let pc = 0;
	let sp = 0;
	let fp = 0;
	let cont = 1;
	init(heap, 47);
	setup_interupts();
	setup_cgbase();
	setup_pocesses();
	add_new_pcb("usr5.exe"); 
	out("I'm the operating system!\n");
	assembly
	{ 
		store pc, [<pc>]
		store sp, [<sp>]
		store fp, [<fp>] 
	}
	out("pc = %x, sp = %x, fp = %x\n", pc, sp, fp);
	out("  start = %x\n", start);
	out("     @x = %x\n", @x);
	out("      y = %d\n", y);
	print_mem_map();
	while cont = 1 do
	{
		user_interface();
		outs("continue? (1/0): ");
		cont := inno()
	}
	outs("all done\n")
}


