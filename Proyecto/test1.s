program
var
	int x,y;
begin
	x := 5;
	y := 0;
	while y<10 do
		x := -x*2;
		y := y+1;
	end;
	print x;
end
