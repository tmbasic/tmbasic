type Foo
    child as List of Foo
end type

sub Main()
end sub

--output--
Compiler error
kRecursiveRecordType
Foo
2:22
The type "Foo" contains a recursive type reference which is not allowed. The cycle is: foo -> list of foo -> foo
