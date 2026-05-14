## Project3

In Part A (Tasks 1-3), you’ll be wiring up the ALU and RegFile for a basic RISC-V CPU, as well as implementing the CPU datapath for executing addi instructions. In Part B (Tasks 4-5), you’ll use these components (and others) to wire up a working CPU that runs actual RISC-V instructions!


### Part A
- Wire up the ALU and RegFile 
- Implement the CPU datapath for executing addi instructions 

#### For alu
- there is three inputs: a,b,alu_sel. a and b is 32 bits, alu_sel is 4 bits.
- there is one output: out, which is 32 bits.
- 我们将实现15个ALU操作，具体见文档，思路是用input a和b将所有操作的结果都算出来，然后根据alu_sel选择输出哪一个。
- 对于加法add,它的carryout我们不需要保存。
- 对于srl和sra,如何设计呢？首先考虑到由于a一共就32位，那么我们最多也就是右移31位，也就是b的前5位。我们可以分别看b的前5位，
如果对应的位是1,那么就右移1,2,4,8,16位。b的更高位如果是1,那么直接是0了。OH, 原来logisim中自带shift模块，笑死。

### Part B
- wire up a working CPU that runs actual RISC-V instructions with the components that we have built in Part A

### Run 
```
export _JAVA_AWT_WM_NONREPARENTING=1
java -jar ./logisim-evolution.jar
```

### Constraints
- Some tools not allowd: Transistor, Transmission Gate, POR, Pull Resistor, Power, Ground, POR, Divider, RAM, Random Generator


### Note
#### PART A
- 当前已经完成了part A的部分，客观而言，虽然还没有完成part B,但是我能感受到这个project的任务量非常大。
对于part A来说alu的实现还算好的，regfile的实现简直要命，32个register,可想而知连线的困难。。。
cpu部分由于测试文件较多，并且各部分之间关系复杂，在动手之前最好理清run.circ,test_harness.circ,cpu.circ之间的关系。
