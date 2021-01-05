#include "sovle.h" 
 
 
int applicableMoves[] = { 0, 262143/*18个1*/, 259263/*111111010010111111*/, 74943/*10010010010111111*/, 74898/*10010010010010010*/ };
 
// TODO: Encode as strings, e.g. for U use "ABCDABCD"
 
int affectedCubies[][8] = {						//对每一个块编码，前四位棱块，后四位角块（顶层右下角开始0.1.2.3，底层右下角开始4.5.6.7），相同块数字相同。
	{ 0,  1,  2,  3,  0,  1,  2,  3 },   // U
	{ 4,  7,  6,  5,  4,  5,  6,  7 },   // D
	{ 0,  9,  4,  8,  0,  3,  5,  4 },   // F
	{ 2, 10,  6, 11,  2,  1,  7,  6 },   // B
	{ 3, 11,  7,  9,  3,  2,  6,  5 },   // L
	{ 1,  8,  5, 10,  1,  0,  4,  7 },   // R
};
 
/********************************************************************************
*对state状态进行旋转(顺时针90°180°270°)，返回旋转后的状态
*
*move=0时，U面顺时针旋转90°move=1时，U面顺时针旋转180°move=2时，U面顺时针旋转270°
*move=3时，D面顺时针旋转90°move=4时，D面顺时针旋转180°move=5时，D面顺时针旋转270°
*move=6时，F面顺时针旋转90°move=7时，F面顺时针旋转180°move=8时，F面顺时针旋转270°
*move=9时，B面顺时针旋转90°move=10时，B面顺时针旋转180°move=11时，B面顺时针旋转270°
*move=12时，L面顺时针旋转90°move=13时，L面顺时针旋转180°move=14时，L面顺时针旋转270°
*move=15时，R面顺时针旋转90°move=16时，R面顺时针旋转180°move=17时，R面顺时针旋转270°
*
**********************************************************************************/
vi applyMove(int move, vi state) {
	int turns = move % 3 + 1;			//move对3求余+1  旋转90°的次数
	int face = move / 3;				//move除3取整    定义旋转哪一个面
	while (turns--) {					/*顺时针旋转turns个90°*/
		vi oldState = state;
		for (int i = 0; i<8; i++) {			/*在旋转过程中分别对8个楞块和8个角块的方向进行赋值*/
			int isCorner = i > 3;			//将i>3的逻辑判断结果（0，1）赋给isCorner i>3才能取到affectedCubies中的后四位，即角块
			int target = affectedCubies[face][i] + isCorner * 12;
			int killer = affectedCubies[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * 12;;		//将面按顺序的下一个楞块或者角块的值取出来（用于移位）
			int orientationDelta = (i<4) ? (face>1 && face<4) : (face<2) ? 0 : 2 - (i & 1);			//顺时针旋转后方向改变量（0.1.2）
			state[target] = oldState[killer];														//用后一个替换前一个，完成顺时针旋转
			state[target + 20] = oldState[killer + 20] + orientationDelta;							//记录旋转后方向的值
			if (!turns)			/*如果turns!=0即还要旋转，则不进入；若turns==0，则进入求余，防止方向值超过（0.1）或（0.1.2）*/
				state[target + 20] %= 2 + isCorner;					//楞块和2求余，角块和3求余，不改变方向的值
		}
	}
	return state;
}
/*用于返回move的逆动作*/
int inverse(int move) {
	return move + 2 - 2 * (move % 3);
}
//----------------------------------------------------------------------
int phase;				//整个西斯尔思韦特的步骤
//----------------------------------------------------------------------
				/*取出输入状态的方向的值id*/
vi id(vi state) {
 
	//--- Phase 1: Edge orientations.（//第一步：棱块取向）
	if (phase < 2)
		return vi(state.begin() + 20, state.begin() + 32);			//返回输入state状态的棱块的取向，共12位，0表示方向正确，1表示方向错误（即翻转了180°）
 
	//-- Phase 2: Corner orientations, E slice edges.（//第二步：角块方向，E层（即中间层）棱块）
	if (phase < 3) {
		vi result(state.begin() + 31, state.begin() + 40);			//取角块的方向值给result
		for (int e = 0; e<12; e++)
			result[0] |= (state[e] / 8) << e;						// result[0]用于存E层（中间层）楞块的位置（用二进制表示）
		return result;												//返回角块的方向（0.1.2）和E层楞块的位置（result[0]）
	}
 
	//--- Phase 3: Edge slices M and S, corner tetrads, overall parity.（//第三步：M层S层的楞块，对应角块呈现正四面体型）
	if (phase < 4) {
		vi result(3);
		for (int e = 0; e<12; e++)
			result[0] |= ((state[e] > 7) ? 2 : (state[e] & 1)) << (2 * e);	//result[0]用24位存12个楞块位置正确，
		for (int c = 0; c<8; c++)
			result[1] |= ((state[c + 12] - 12) & 5) << (3 * c);				//result[1]用24位存放8个角块的位置
		for (int i = 12; i<20; i++)
			for (int j = i + 1; j<20; j++)
				result[2] ^= state[i] > state[j];							//result[2]=0表示角块方向正确，result[2]=1表示角块方向错误
		return result;
	}
 
	//--- Phase 4: The rest.
	return state;
}
 
//----------------------------------------------------------------------
//输入魔方U:黄 D:白 L:蓝 R:绿 F:红 B:橙 
int main(vector<string> argv) {
	char  * argv[] = { "DB","UR", "UB", "UL", "DF","DR", "UF", "DL", "BR", "FL",  "FR", "BL",
		"UBL","URB", "UFR", "ULF", "DLB", "DFL","DRF",  "DBR" };						//输入魔方的状态，对照目标状态进行输入
 
		//--- Define the goal.（//定义目标魔方楞块和角块的位置）
	string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
		"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };						//前十二位为棱块，后八位为角块
 
		//--- Prepare current (start) and goal state.（//准备当前（开始）和目标状态）
	vi currentState(40), goalState(40);
	for (int i = 0; i<20; i++) {						/*该for循环的作用: 将当前魔方状态输入到数表currentState里，
														由字母顺序转化到数字顺序，字母顺序包涵了位置和方向，
														数字顺序用一位表示位置，一位表示方向。
														规则为：0-11/12-19存按goal里编号楞块/角块的数字位置，
														20-39存楞块和角块的方向
														楞块如需翻转，则20-31对应位置为1；
														角块如需旋转，则顺时针旋转90°记为1，顺时针旋转180°记为2*/
														//--- Goal state.
		goalState[i] = i;			//初始化目标魔方各个楞块和角块的位置
 
		//--- Current (start) state.（//输入魔方各个楞块和角块的位置）
		string cubie = argv[i];						//遍历输入的argv中的20个块
		/*和目标魔方块的位置比较，块需要顺时针旋转90°或者楞块翻转180°*/
		while ((currentState[i] = find(goal, goal + 20, cubie) - goal) == 20) {
			cubie = cubie.substr(1) + cubie[0];				//旋转后块的字母顺序
			currentState[i + 20]++;							//记录到方向，回到正确位置需要顺时针旋转180°为2，顺时针旋转90°为1
		}
	}
 
	//--- Dance the funky Thistlethwaite...（//开始牛逼的西斯尔思韦特操作）
	while (++phase < 5) {							//开始循环五个过程
 
		//--- Compute ids for current and goal state, skip phase if equal.（//计算当前和目标状态的方向取值id，如果相等(表明方向正确，不需要调整)则跳过）
		vi currentId = id(currentState), goalId = id(goalState);
		if (currentId == goalId)
			continue;
 
		//--- Initialize the BFS queue.（//初始化BFS（广度优先）队列（先进先出））
		queue<vi> q;								//定义一个队列q，用于存放不同状态
		q.push(currentState);						//push入队，当前状态表入队
		q.push(goalState);							//push入队，目标状态表入队
 
		//--- Initialize the BFS tables.（//初始化BFS算法的图表	map通过平衡二叉树对节点进行存储）
		map<vi, vi> predecessor;						//旋转前后的状态表存进predecessor，旋转后的表前面出现过则不存（即状态等价不存）
		map<vi, int> direction, lastMove;				//direction：存放不同状态的方向，该状态由输入魔方旋转得到，则关键字为1；有目标魔方旋转得到，关键字为2
														//lastMove：将旋转后的方向值存入并记录当时的move值(即旋转的方式）
		direction[currentId] = 1;
		direction[goalId] = 2;
 
		//--- Dance the funky bidirectional BFS...（//开始牛逼的BFS算法）
		while (1) {
 
			//--- Get state from queue, compute its ID and get its direction.（//从队列获取状态，计算它的ID并得到它的方向）
			vi oldState = q.front();
			q.pop();
			vi oldId = id(oldState);
			int& oldDir = direction[oldId];
 
			//--- Apply all applicable moves to it and handle the new state.（//将所有适用的动作(每个面旋转90.180.270)应用到它并记录新的状态）
			for (int move = 0; move<18; move++) {
				if (applicableMoves[phase] & (1 << move)) {							//在phase=2时，控制FB面只能旋转180°即降群到<U,D,F2,B2,L,R>
																					//在phase=3时，控制FBLR面只能旋转180°即降群到<U,D,F2,B2,L2,R2>
																					//在phase=4时，控制UDFBLR面只能旋转180°即降群到<U2,D2,F2,B2,L2,R2>
					//--- Apply the move.（//旋转）
					vi newState = applyMove(move, oldState);			//旋转后的状态
					vi newId = id(newState);							//旋转后的状态各个楞块和角块的方向
					int& newDir = direction[newId];						//拥有新方向的状态是否出现过，是，则返回关键字给newDir；否，则以关键字为0存入direction
 
					//--- Have we seen this state (id) from the other direction already?（//判断该状态是否出现过）
					//--- I.e. have we found a connection?（//判断是否能和关键字为2的状态联系起来，如果能，则找到解法，否，则继续搜索）
					if (newDir  &&  newDir != oldDir) {								//由目标魔方旋转后的状态的方向值与输入魔方某一状态的方向值相等时if成立
 
						//--- Make oldId represent the forwards and newId the backwards search state.（//oldId表示之前的状态的方向，newId表示旋转后的状态的方向值，搜索解法）
						if (oldDir > 1) {
							swap(newId, oldId);
							move = inverse(move);
						}
 
						//--- Reconstruct the connecting algorithm.（//重现联系这两个状态的步骤move）
						vi algorithm(1, move);										//用于存放步骤
						while (oldId != currentId) {								//在predecessor表里查找oldId==currentId，并记录需要的步骤到algorithm（算法）
																					//即输入魔方按照algorithm的步骤旋转就可到达目标魔方旋转后的状态（即联系direction的关键字1和2）
							algorithm.insert(algorithm.begin(), lastMove[oldId]);
							oldId = predecessor[oldId];
						}
						while (newId != goalId) {									//还原到目标魔方状态需要转动的步骤
							algorithm.push_back(inverse(lastMove[newId]));
							newId = predecessor[newId];
						}
 
						//--- Print and apply the algorithm.（//打印并应用该算法）	serial_write(going_write[i]);
						for (int i = 0; i<(int)algorithm.size(); i++) {
							cout << "UDFBLR"[algorithm[i] / 3] << algorithm[i] % 3 + 1;		//打印需要旋转的面和角度，1.2.3顺时针旋转90.180.270
							//serial_write(algorithm[i]);										//用数字代表该转的面，自己找规律
							answer.push_back(algorithm[i]);									//把结果存入answer
 
							//serial_write("UDFBLR"[algorithm[i] / 3]);
							//serial_write(algorithm[i] % 3 + 1);
							currentState = applyMove(algorithm[i], currentState);			//旋转后的值赋给currentState（当前值）
						}
 
						//--- Jump to the next phase.
						goto nextPhasePlease;				//进入牛逼的西斯尔思韦特的下一步
					}
 
					//--- If we've never seen this state (id) before, visit it.
					if (!newDir) {
						q.push(newState);
						newDir = oldDir;
						lastMove[newId] = move;
						predecessor[newId] = oldId;
					}
				}
			}
		}
	nextPhasePlease:
		;
	}
	return 0;
}
