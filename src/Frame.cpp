#include "types.h"
#include "opcodes.h"
#include "JavaClass.h"
#include <string>
#include "ExecutionEngine.h"
#include <assert.h>
#include "Frame.h"
#include "ObjectHeap.h"

int Frame::exec(ExecutionEngine* engine) {
	u1* bc = this->pMethod->pCode_attr->code + this->pc;

	i4 error = 0;
	JavaClass* pClass = this->pClass;
	std::string strMethod;
	pClass->GetStringFromConstPool(this->pMethod->name_index, strMethod); 
	printf("Execute At Class %s Method %s \n", (pClass->GetName()).c_str() , strMethod.c_str());
	i4 index = 0;
	i8 longVal;
	while (1)
	{

		DbgPrint(("Stack values "));
		for (int i = 0; i < this->sp + this->stack - Frame::pOpStack + 1; i++)
		{
			DbgPrint(("[%d] "), Frame::pOpStack[i].intValue);
		}
		DbgPrint(("\n"));
		DbgPrint(("Opcode = %s [%d] Stack=%d [+%d]\n"), OpcodeDesc[(u1)bc[this->pc]], (u1)bc[this->pc], this->sp, this->stack - Frame::pOpStack);
		switch (bc[this->pc])
		{
		case nop:
			this->pc++;
			break;

			///////////////// Stack Operations ////////////////
			//Instructions that push a constant onto the stack
		case iconst_m1:
		case iconst_0:
		case iconst_1:
		case iconst_2:
		case iconst_3:
		case iconst_4:
		case iconst_5:
			this->sp++;
			this->stack[this->sp].intValue = (u1)bc[this->pc] - iconst_0;
			this->pc++;
			break;

		case aconst_null:
			this->sp++;
			this->stack[this->sp].object.heapPtr = 0;
			this->pc++;
			break;

		case lconst_0:// 9 /*(0x9)*/ 
		case lconst_1:// 10 /*(0xa)*/
			this->sp++;
			this->stack[this->sp].intValue = 0;
			this->sp++;
			this->stack[this->sp].intValue = (u1)bc[this->pc] - lconst_0;
			this->pc++;
			break;

		case bipush:// 16 /*(0x10)*/
			this->sp++;
			this->stack[this->sp].charValue = (u1)bc[this->pc + 1];
			this->pc += 2;
			break;
		case sipush:// 17 /*(0x11)*/
			this->sp++;
			this->stack[this->sp].shortValue = getu2(&bc[this->pc + 1]);
			this->pc += 3;
			break;

		case ldc: //Push item from constant pool 			
			this->stack[++this->sp] = engine->LoadConstant(this->pClass, (u1)bc[this->pc + 1]);
			this->pc += 2;
			break;

		case ldc2_w:// 20 /*(0x14)*/
			/*
			ldc2_w	14	0001 0100	2: indexbyte1, indexbyte2	→ value

			push a constant #index from a constant pool (double, long, or a dynamically-computed constant) onto the
			stack (wide index is constructed as indexbyte1 << 8 + indexbyte2)

			*/

			index = getu2(&bc[this->pc + 1]);
			this->sp++;
			this->stack[this->sp].intValue = getu4(&((char*)pClass->constant_pool[index])[1]);
			this->sp++;
			this->stack[this->sp].intValue = getu4(&((char*)pClass->constant_pool[index])[5]);
			this->pc += 3;
			break;

			//Instructions that load a local variable onto the stack
		case aload:// 25 /*(0x19)*/
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc + 1]];
			this->pc += 2;
			break;

		case iload:// 21 /*(0x15)*/
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc + 1]];
			this->pc += 2;
			break;
		case iload_0: //26 Load int from local variable 0 
		case iload_1: //27 Load int from local variable 1 
		case iload_2: //28 Load int from local variable 2 
		case iload_3: //29 Load int from local variable 3 
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc] - iload_0];
			this->pc++;
			break;


		case lload:// 22 /*(0x16)*/

			break;
		case lload_0:// 30 /*(0x1e) */
		case lload_1:// 31 /*(0x1f) */
		case lload_2:// 32 /*(0x20) */
		case lload_3:// 33 /*(0x21) */
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc] - lload_0];
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc] - lload_0 + 1];
			this->pc++;
			break;

		case fload_0: // 34 /*(0x22)*/ 
		case fload_1: // 35 /*(0x23) */
		case fload_2: // 36 /*(0x24) */
		case fload_3: // 37 /*(0x25)*/
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc] - fload_0];
			this->pc++;
			break;

		case aload_0:  //42 Load reference from local variable 0
		case aload_1:  //Load reference from local variable 1
		case aload_2:  //Load reference from local variable 2
		case aload_3:  //Load reference from local variable 3
			this->sp++;
			this->stack[this->sp] = this->stack[(u1)bc[this->pc] - aload_0];
			engine->DumpObject(this->stack[this->sp].object);
			this->pc++;
			break;

		case iaload:// 46 /*(0x2e)*/Load int from array
			//..., arrayref, index  => ..., value
			this->stack[this->sp - 1] = engine->pObjectHeap->GetObjectPointer(this->stack[this->sp - 1].object)[this->stack[this->sp].intValue + 1];
			this->sp--;
			this->pc++;
			break;
		case aaload://50 
			//..., arrayref, index  ..., value
			this->stack[this->sp - 1] = engine->pObjectHeap->GetObjectPointer(this->stack[this->sp - 1].object)[this->stack[this->sp].intValue + 1];
			this->sp--;
			this->pc++;
			break;
			//Instructions that store a value from the stack into a local variable
		case astore:// 58 (0x3a)
			this->stack[(u1)bc[this->pc + 1]] = this->stack[this->sp--];
			this->pc += 2;
			break;

		case istore:// 54 /*(0x36)*/
			this->stack[(u1)bc[this->pc + 1]] = this->stack[this->sp--];
			this->pc += 2;
			break;
		case istore_0: // 59 /*(0x3b)*/ 
		case istore_1: // 60 /*(0x3c) */
		case istore_2: // 61 /*(0x3d) */
		case istore_3: // 62 /*(0x3e)*/			
			this->stack[(u1)bc[this->pc] - istore_0] = this->stack[this->sp--];
			this->pc++;
			break;

		case lstore_0: // 63 /*(0x3f) */
		case lstore_1: // 64 /*(0x40) */
		case lstore_2: // 65 /*(0x41) */
		case lstore_3: // 66 /*(0x42) */
			this->stack[(u1)bc[this->pc] - lstore_0 + 1] = this->stack[this->sp--];
			this->stack[(u1)bc[this->pc] - lstore_0] = this->stack[this->sp--];
			this->pc++;
			break;

		case fstore_0:
		case fstore_1:
		case fstore_2:
		case fstore_3:
			this->stack[(u1)bc[this->pc] - fstore_0] = this->stack[this->sp--];
			this->pc++;
			break;

		case astore_0:// 75 /*(0x4b) Store reference into local variable 0*/
		case astore_1:// 76 /*(0x4c) */
		case astore_2:// 77 /*(0x4d) */
		case astore_3:// 78 /*(0x4e)*/
			this->stack[(u1)bc[this->pc] - astore_0] = this->stack[this->sp--];
			this->pc++;
			break;

		case iastore:// 79 /*(0x4f)*/
		case aastore: // 83 - both seems same (TODO: Check it)
			engine->pObjectHeap->GetObjectPointer(this->stack[this->sp - 2].object)[this->stack[this->sp - 1].intValue + 1] = this->stack[this->sp];
			this->sp -= 3;
			this->pc++;
			break;

			//Generic (typeless) stack operations

		case dup:// 89 /*(0x59)*/
			this->stack[this->sp + 1] = this->stack[this->sp];
			this->sp++;
			this->pc++;
			break;
		case dup_x1:// 90 /*(0x5a)*/
			this->stack[this->sp + 1] = this->stack[this->sp];
			this->stack[this->sp] = this->stack[this->sp - 1];
			this->stack[this->sp - 1] = this->stack[this->sp + 1];
			this->sp++;
			this->pc++;
			break;
		case dup_x2:// 91 /*(0x5b)*/
			error = 1;
			break;

			//Type Conversion

			//Integer Arithmetic 

		case iadd: //96
			this->stack[this->sp - 1].intValue = this->stack[this->sp - 1].intValue + this->stack[this->sp].intValue;
			this->sp--;
			this->pc++;
			break;
		case ladd:// 97 /*(0x61)*/
			longVal = (i8)(((i8)this->stack[this->sp - 3].intValue << 32) | (i8)this->stack[this->sp - 2].intValue) + (i8)(((i8)this->stack[this->sp - 1].intValue << 32) | (i8)this->stack[this->sp].intValue);
			this->stack[this->sp - 3].intValue = HIINT64(longVal);
			this->stack[this->sp - 2].intValue = LOINT64(longVal);
			this->sp -= 2;
			this->pc++;
			break;
		case isub: //100
			this->stack[this->sp - 1].intValue = this->stack[this->sp - 1].intValue - this->stack[this->sp].intValue;
			this->sp--;
			this->pc++;
			break;
		case imul://104
			this->stack[this->sp - 1].intValue = this->stack[this->sp - 1].intValue * this->stack[this->sp].intValue;
			this->sp--;
			this->pc++;
			break;

		case iinc:// 132 /*(0x84)*/ Increment local variable by constant
			this->stack[(u1)bc[this->pc + 1]].intValue += (char)bc[this->pc + 2];
			this->pc += 3;
			break;
			////////////////////// Logic ///////////////////

			//Shift operations

			//Bitwise boolean operations

			//Floating Point Arithmetic 



			/////////////// Objects and Arrays  ////////////

			//Instructions that deal with objects

		case _new:// 187 (0xbb)
			engine->ExecuteNew(this);
			this->pc += 3;
			break;
		case putfield: //181 (0xb5): Set field in object
			engine->PutField(this);
			this->sp -= 2;
			this->pc += 3;
			break;

		case getfield: //180 (0xb4) Fetch field from object
			engine->GetField(this);
			this->pc += 3;
			break;

			//Instructions that deal with arrays

		case newarray:// 188 /*(0xbc)*/
			engine->ExecuteNewArray(this);
			this->pc += 2;
			break;

		case anewarray: //189
			engine->ExecuteANewArray(this);
			this->pc += 3;
			break;
			////////////// Control Flow /////////////////////

			//Conditional branch instructions

		case if_icmpeq: // 159 /*(0x9f) */
			if (this->stack[this->sp - 1].intValue == this->stack[this->sp].intValue)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp -= 2;
			break;
		case if_icmpne: //160 /*(0xa0) */
			if (this->stack[this->sp - 1].intValue != this->stack[this->sp].intValue)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp -= 2;
			break;
		case if_icmplt: // 161 /*(0xa1) */
			if (this->stack[this->sp - 1].intValue < this->stack[this->sp].intValue)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp -= 2;
			break;
		case if_icmpge: // 162 /*(0xa2) */
			if (this->stack[this->sp - 1].intValue >= this->stack[this->sp].intValue)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp -= 2;
			break;
		case if_icmpgt: // 163 /*(0xa3) */
			if (this->stack[this->sp - 1].intValue > this->stack[this->sp].intValue)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp -= 2;
			break;
		case if_icmple: // 164 /*(0xa4)*/
			if (this->stack[this->sp - 1].intValue <= this->stack[this->sp].intValue)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp -= 2;
			break;

		case ifeq:// 153 /*(0x99) */
			if (this->stack[this->sp].intValue == 0)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp--;
			break;
		case ifne:// 154 /*(0x9a) */
			if (this->stack[this->sp].intValue != 0)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp--;
			break;
		case iflt:// 155 /*(0x9b) */
			if (this->stack[this->sp].intValue < 0)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp--;
			break;
		case ifge:// 156 /*(0x9c) */
			if (this->stack[this->sp].intValue >= 0)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp--;
			break;
		case ifgt:// 157 /*(0x9d) */
			if (this->stack[this->sp].intValue > 0)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp--;
			break;
		case ifle:// 158 /*(0x9e)*/
			if (this->stack[this->sp].intValue <= 0)
			{
				this->pc += geti2(&bc[this->pc + 1]);
			}
			else
			{
				this->pc += 3;
			}
			this->sp--;
			break;
			//Comparison instructions

			//Unconditional branch instructions
		case  _goto: // 167 /*(0xa7)*/
			this->pc += geti2(&bc[this->pc + 1]);
			break;
			//Table jumping instructions

			////////////// Exceptions ///////////////////////
		case athrow:
			error = 1;
			break;

			//////////////////////// Method Invocation and Return ////////

			//Method invocation instructions
		case invokespecial:
			engine->ExecuteInvokeSpecial(this);
			this->pc += 3;
			break;

		case invokevirtual: //182
			engine->ExecuteInvokeVirtual(this, invokevirtual);
			this->pc += 3;
			break;

		case invokestatic:// 184 
			engine->ExecuteInvokeVirtual(this, invokestatic);
			this->pc += 3;
			break;
			//Method return instructions
		case ireturn: //172 (0xac)			
			DbgPrint(("----IRETURN------\n"));
			this->stack[0].intValue = this->stack[this->sp].intValue;
			return ireturn;
			break;

		case _return: //177 (0xb1): Return (void) from method			
			DbgPrint(("----RETURN------\n"));
			return 0;//METHOD_RETURN;
			break;
			//////////////// Thread Synchronization ////////////////////

		case monitorenter:// Enter and acquire object monitor 
		case monitorexit:// Release and exit object monitor
			error = 1;
			break;

		default:
			error = 1;
			break;
		}

		if (error) break;
	}

	assert(!error);
}