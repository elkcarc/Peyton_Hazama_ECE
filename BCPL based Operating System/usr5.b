import "io"

let wasted = vec(1000)

let twon(n) be
{ 
	if n = 0 then
		resultis 0;
  resultis 2 + twon(n-1); 
}

let syscall(n, a, b, c, d) be
{ 
	assembly
	{ 
		load r1, [<a>]
		load r2, [<b>]
		load r3, [<c>]
		load r4, [<d>]
		syscall [<n>] 
	} 
}

let sc2() be
{ 
	assembly { syscall 2 } 
}

let start() be
{ 
	let x = 9;
	let y = x*x;
	out("I'm only a user program\n");
	out("  start = %x\n", start);
	out("     @x = %x\n", @x);
	out("      y = %d\n", y);
	out("twon(100) = %d\n", twon(100));
	syscall(1, 123, 234, 345, 456);
	sc2(999,888,777,666);
	out("twon(200) = %d\n", twon(200));
	out("twon(300) = %d\n", twon(300));
	out("twon(400) = %d\n", twon(400));
	out("twon(500) = %d\n", twon(500)); 
}


