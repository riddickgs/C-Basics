Structure is a user-defined datatype which can store members of different datatypes together.

struct <Tag> {
  type1 member1;
  type2 member2;
  .
  .
  typeM memberN;
}<varible name>;

Note: size of structure will be sum of sizes of all members datatypes i.e. sizeof(type1)+sizeof(type2)+ ... + sizeof(typeM)
     Concepts to remember here
	1. Structure padding
	2. Structure packing
	3. Memory Alignment (Memory management)
