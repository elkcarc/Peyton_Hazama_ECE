import "io"

static { free_page_list, free_page_ptr }

manifest
{ page_address_only = bitnot 0x7FF }

let get_page() be
{ let addr = free_page_list ! free_page_ptr;
  if addr = 0 then
  { outs("\nRun out of free pages\n");
    finish } 
  free_page_ptr +:= 1;
  resultis addr }

let find_free_pages() be
{ let begin = ((! 0x101) >> 11) + 1;
    // begin = first (whole) free page number.
  let end = (! 0x100) >> 11;
    // end = first non-existent page number.
  let num_free = end - begin - 1;
  let pages_needed = (end >> 11) + 1;
    // how many pages needed to store the entire list of free pages
  let free_list = begin << 11;
    // free_list = ptr to list of free pages
  let first_free_page = begin + pages_needed;
  let i, pn, num_entries = pages_needed << 11;
  out("free page list stored at %x to %x\n", free_list, free_list + num_free - 1);
  // next put addresses of free pages in free page list
  i := 0;
  while i < num_free do
  { free_list ! i := (first_free_page + i) << 11;
    i +:= 1 }
  pn := 1;
  while pn < begin do
  { free_list ! i := pn << 11;
    i +:= 1;
    pn +:= 1 }
  while i < num_entries do
  { free_list ! i := 0;
    i +:= 1 }
  resultis free_list }

let release_page(addr) be
{ addr := addr bitand page_address_only;
  free_page_ptr -:= 1;
  free_page_list ! free_page_ptr := addr }

let load_program(fname, page_table) be
{ let page_num = -1;
  let buff_pos = 2048;
  let page_addr;
  let r = devctl(DC_TAPE_LOAD, 1, fname, 'R');
  if r < 0 then
  { out("error %d for load '%s'\n", r, fname);
    finish }

  while true do
  { if  buff_pos = 2048 then
    { buff_pos := 0;
      page_num +:= 1;
      page_addr := get_page();
      page_table ! page_num := page_addr bitor 1 }
    r := devctl(DC_TAPE_READ, 1, page_addr + buff_pos);
    if r < 512 then break;
    buff_pos +:= 128 }

  devctl(DC_TAPE_UNLOAD, 1) }

let set_up_process(osname) be
{ let pdbr_value = get_page();    //   v C0000000
  let pt_usr_code = get_page();   //   v C0800000
  let pt_usr_stack = get_page();  //   v C08FF800
  let pt_os_code = get_page();    //   v C0900000
  let pt_os_stack = get_page();   //   v C097F800
  let pt_os_attic = get_page();   //   v C0980000
  let pt_os_freepages = get_page(); // v C0980800
  let page_addr;
  assembly
  { CLRPP [<pdbr_value>]
    CLRPP [<pt_os_code>]
    CLRPP [<pt_os_stack>]
    CLRPP [<pt_os_attic>]
    CLRPP [<pt_os_freepages>]
    CLRPP [<pt_usr_stack>]
    CLRPP [<pt_usr_code>] }
  pdbr_value ! 770 := pdbr_value bitor 1;
  pdbr_value ! 769 := pt_os_freepages bitor 1;
  pdbr_value ! 768 := pt_os_attic bitor 1;
  pdbr_value ! 767 := pt_os_stack bitor 1;
  pdbr_value ! 512 := pt_os_code bitor 1;
  pdbr_value ! 511 := pt_usr_stack bitor 1;
  pdbr_value !   0 := pt_usr_code bitor 1;
  page_addr := get_page();     // v BFFFF800
  pt_os_stack ! 2047 := page_addr bitor 1;
  page_addr := get_page();     // v 7FFFF800
  pt_usr_stack ! 2047 := page_addr bitor 1;
  pt_os_attic ! 0 := pdbr_value bitor 1;
  page_addr := get_page();     // v C0000800
  pt_os_attic ! 1 := page_addr bitor 1;
  assembly
  { load  r1, [<page_addr>]
    add   r1, 0x10
    setsr r1, $INTVEC         // v C0000810
    add   r1, 0x20
    setsr r1, $CGBR           // v C0000830
    load  r1, 0x100
    setsr r1, $CGLEN }
  pt_os_freepages ! 0 := free_page_list bitor 1;
  load_program(osname, pt_os_code);
  load_program("usr5.exe", pt_usr_code);
  page_addr ! 0 := free_page_ptr;       // first entry in attic page
  page_addr ! 1 := page_addr;     // 2nd entry = physical address of attic page

  resultis pdbr_value }

let print_mem_map(pdbr) be
{ out("00000000 to FFFFFFFF: PDBR at %x\n", pdbr);
  for ptn = 1023 to 0 by -1 do          // page table number
  { let pde = pdbr ! ptn;               // page directory entry
    let pt = pde >> 11 << 11;           // page table's address
    let ptrange = ptn << 22;            // beginning of PT's VA range
    if pde = 0 then loop;
    out("   %08x to %08x: ", ptrange, ptrange + 0x3FFFFF);
    if (pde bitand 1) = 0 then
    { out("%08x - invalid\n", pde);
      loop }
    out("pp %d at %x\n", pde >> 11, pde >> 11 << 11);
    for pn = 2047 to 0 by -1 do         // page number in table
    { let pte = pt ! pn;                // page table entry
      let range = ptrange + (pn << 11); // beginning of page's VA range
      if pte = 0 then loop;
      out("      %08x to %08x: ", range, range + 0x7FF);
      if (pte bitand 1) = 0 then
      { out("%08x - invalid\n", pte);
        loop }
      out("pp %d at %x\n", pte >> 11, pte >> 11 << 11) } } }

let start() be
{ let pdbr_value, start_address, stack_ptr;
  free_page_list := find_free_pages();
  free_page_ptr := 0;

  pdbr_value := set_up_process("os5.exe");
  print_mem_map(pdbr_value);
  start_address := 0x80000000;
  stack_ptr := 0xBFFFFFFF;
  assembly
  { LOAD   r1, [<pdbr_value>]
    SETSR  r1, $PDBR
    GETSR  r1, $FLAGS
    SBIT   r1, $VM
    LOAD   r2, [<start_address>]
    LOAD   sp, [<stack_ptr>]
    FLAGSJ r1, r2 } }


