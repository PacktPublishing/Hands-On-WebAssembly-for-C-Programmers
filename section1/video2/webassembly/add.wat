(module 
	(func (param $x i32) (param $y i32) (result i32) 
		(i32.add
			(get_local $x) 
			(get_local $y)
		)
	)
	(func (param $x i32) (param $y i32) (result i32) 
		get_local $x
		get_local $y
		i32.sub 
	)
	(export "my_add" (func 0))
	(export "my_sub" (func 1))
) 
