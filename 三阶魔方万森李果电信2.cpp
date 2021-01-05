#include "sovle.h" 
 
 
int applicableMoves[] = { 0, 262143/*18��1*/, 259263/*111111010010111111*/, 74943/*10010010010111111*/, 74898/*10010010010010010*/ };
 
// TODO: Encode as strings, e.g. for U use "ABCDABCD"
 
int affectedCubies[][8] = {						//��ÿһ������룬ǰ��λ��飬����λ�ǿ飨�������½ǿ�ʼ0.1.2.3���ײ����½ǿ�ʼ4.5.6.7������ͬ��������ͬ��
	{ 0,  1,  2,  3,  0,  1,  2,  3 },   // U
	{ 4,  7,  6,  5,  4,  5,  6,  7 },   // D
	{ 0,  9,  4,  8,  0,  3,  5,  4 },   // F
	{ 2, 10,  6, 11,  2,  1,  7,  6 },   // B
	{ 3, 11,  7,  9,  3,  2,  6,  5 },   // L
	{ 1,  8,  5, 10,  1,  0,  4,  7 },   // R
};
 
/********************************************************************************
*��state״̬������ת(˳ʱ��90��180��270��)��������ת���״̬
*
*move=0ʱ��U��˳ʱ����ת90��move=1ʱ��U��˳ʱ����ת180��move=2ʱ��U��˳ʱ����ת270��
*move=3ʱ��D��˳ʱ����ת90��move=4ʱ��D��˳ʱ����ת180��move=5ʱ��D��˳ʱ����ת270��
*move=6ʱ��F��˳ʱ����ת90��move=7ʱ��F��˳ʱ����ת180��move=8ʱ��F��˳ʱ����ת270��
*move=9ʱ��B��˳ʱ����ת90��move=10ʱ��B��˳ʱ����ת180��move=11ʱ��B��˳ʱ����ת270��
*move=12ʱ��L��˳ʱ����ת90��move=13ʱ��L��˳ʱ����ת180��move=14ʱ��L��˳ʱ����ת270��
*move=15ʱ��R��˳ʱ����ת90��move=16ʱ��R��˳ʱ����ת180��move=17ʱ��R��˳ʱ����ת270��
*
**********************************************************************************/
vi applyMove(int move, vi state) {
	int turns = move % 3 + 1;			//move��3����+1  ��ת90��Ĵ���
	int face = move / 3;				//move��3ȡ��    ������ת��һ����
	while (turns--) {					/*˳ʱ����תturns��90��*/
		vi oldState = state;
		for (int i = 0; i<8; i++) {			/*����ת�����зֱ��8������8���ǿ�ķ�����и�ֵ*/
			int isCorner = i > 3;			//��i>3���߼��жϽ����0��1������isCorner i>3����ȡ��affectedCubies�еĺ���λ�����ǿ�
			int target = affectedCubies[face][i] + isCorner * 12;
			int killer = affectedCubies[face][(i & 3) == 3 ? i - 3 : i + 1] + isCorner * 12;;		//���水˳�����һ�������߽ǿ��ֵȡ������������λ��
			int orientationDelta = (i<4) ? (face>1 && face<4) : (face<2) ? 0 : 2 - (i & 1);			//˳ʱ����ת����ı�����0.1.2��
			state[target] = oldState[killer];														//�ú�һ���滻ǰһ�������˳ʱ����ת
			state[target + 20] = oldState[killer + 20] + orientationDelta;							//��¼��ת�����ֵ
			if (!turns)			/*���turns!=0����Ҫ��ת���򲻽��룻��turns==0����������࣬��ֹ����ֵ������0.1����0.1.2��*/
				state[target + 20] %= 2 + isCorner;					//����2���࣬�ǿ��3���࣬���ı䷽���ֵ
		}
	}
	return state;
}
/*���ڷ���move���涯��*/
int inverse(int move) {
	return move + 2 - 2 * (move % 3);
}
//----------------------------------------------------------------------
int phase;				//������˹��˼Τ�صĲ���
//----------------------------------------------------------------------
				/*ȡ������״̬�ķ����ֵid*/
vi id(vi state) {
 
	//--- Phase 1: Edge orientations.��//��һ�������ȡ��
	if (phase < 2)
		return vi(state.begin() + 20, state.begin() + 32);			//��������state״̬������ȡ�򣬹�12λ��0��ʾ������ȷ��1��ʾ������󣨼���ת��180�㣩
 
	//-- Phase 2: Corner orientations, E slice edges.��//�ڶ������ǿ鷽��E�㣨���м�㣩��飩
	if (phase < 3) {
		vi result(state.begin() + 31, state.begin() + 40);			//ȡ�ǿ�ķ���ֵ��result
		for (int e = 0; e<12; e++)
			result[0] |= (state[e] / 8) << e;						// result[0]���ڴ�E�㣨�м�㣩����λ�ã��ö����Ʊ�ʾ��
		return result;												//���ؽǿ�ķ���0.1.2����E������λ�ã�result[0]��
	}
 
	//--- Phase 3: Edge slices M and S, corner tetrads, overall parity.��//��������M��S�����飬��Ӧ�ǿ�������������ͣ�
	if (phase < 4) {
		vi result(3);
		for (int e = 0; e<12; e++)
			result[0] |= ((state[e] > 7) ? 2 : (state[e] & 1)) << (2 * e);	//result[0]��24λ��12�����λ����ȷ��
		for (int c = 0; c<8; c++)
			result[1] |= ((state[c + 12] - 12) & 5) << (3 * c);				//result[1]��24λ���8���ǿ��λ��
		for (int i = 12; i<20; i++)
			for (int j = i + 1; j<20; j++)
				result[2] ^= state[i] > state[j];							//result[2]=0��ʾ�ǿ鷽����ȷ��result[2]=1��ʾ�ǿ鷽�����
		return result;
	}
 
	//--- Phase 4: The rest.
	return state;
}
 
//----------------------------------------------------------------------
//����ħ��U:�� D:�� L:�� R:�� F:�� B:�� 
int main(vector<string> argv) {
	char  * argv[] = { "DB","UR", "UB", "UL", "DF","DR", "UF", "DL", "BR", "FL",  "FR", "BL",
		"UBL","URB", "UFR", "ULF", "DLB", "DFL","DRF",  "DBR" };						//����ħ����״̬������Ŀ��״̬��������
 
		//--- Define the goal.��//����Ŀ��ħ�����ͽǿ��λ�ã�
	string goal[] = { "UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL",
		"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR" };						//ǰʮ��λΪ��飬���λΪ�ǿ�
 
		//--- Prepare current (start) and goal state.��//׼����ǰ����ʼ����Ŀ��״̬��
	vi currentState(40), goalState(40);
	for (int i = 0; i<20; i++) {						/*��forѭ��������: ����ǰħ��״̬���뵽����currentState�
														����ĸ˳��ת��������˳����ĸ˳�������λ�úͷ���
														����˳����һλ��ʾλ�ã�һλ��ʾ����
														����Ϊ��0-11/12-19�水goal�������/�ǿ������λ�ã�
														20-39�����ͽǿ�ķ���
														������跭ת����20-31��Ӧλ��Ϊ1��
														�ǿ�������ת����˳ʱ����ת90���Ϊ1��˳ʱ����ת180���Ϊ2*/
														//--- Goal state.
		goalState[i] = i;			//��ʼ��Ŀ��ħ���������ͽǿ��λ��
 
		//--- Current (start) state.��//����ħ���������ͽǿ��λ�ã�
		string cubie = argv[i];						//���������argv�е�20����
		/*��Ŀ��ħ�����λ�ñȽϣ�����Ҫ˳ʱ����ת90�������鷭ת180��*/
		while ((currentState[i] = find(goal, goal + 20, cubie) - goal) == 20) {
			cubie = cubie.substr(1) + cubie[0];				//��ת������ĸ˳��
			currentState[i + 20]++;							//��¼�����򣬻ص���ȷλ����Ҫ˳ʱ����ת180��Ϊ2��˳ʱ����ת90��Ϊ1
		}
	}
 
	//--- Dance the funky Thistlethwaite...��//��ʼţ�Ƶ���˹��˼Τ�ز�����
	while (++phase < 5) {							//��ʼѭ���������
 
		//--- Compute ids for current and goal state, skip phase if equal.��//���㵱ǰ��Ŀ��״̬�ķ���ȡֵid��������(����������ȷ������Ҫ����)��������
		vi currentId = id(currentState), goalId = id(goalState);
		if (currentId == goalId)
			continue;
 
		//--- Initialize the BFS queue.��//��ʼ��BFS��������ȣ����У��Ƚ��ȳ�����
		queue<vi> q;								//����һ������q�����ڴ�Ų�ͬ״̬
		q.push(currentState);						//push��ӣ���ǰ״̬�����
		q.push(goalState);							//push��ӣ�Ŀ��״̬�����
 
		//--- Initialize the BFS tables.��//��ʼ��BFS�㷨��ͼ��	mapͨ��ƽ��������Խڵ���д洢��
		map<vi, vi> predecessor;						//��תǰ���״̬����predecessor����ת��ı�ǰ����ֹ��򲻴棨��״̬�ȼ۲��棩
		map<vi, int> direction, lastMove;				//direction����Ų�ͬ״̬�ķ��򣬸�״̬������ħ����ת�õ�����ؼ���Ϊ1����Ŀ��ħ����ת�õ����ؼ���Ϊ2
														//lastMove������ת��ķ���ֵ���벢��¼��ʱ��moveֵ(����ת�ķ�ʽ��
		direction[currentId] = 1;
		direction[goalId] = 2;
 
		//--- Dance the funky bidirectional BFS...��//��ʼţ�Ƶ�BFS�㷨��
		while (1) {
 
			//--- Get state from queue, compute its ID and get its direction.��//�Ӷ��л�ȡ״̬����������ID���õ����ķ���
			vi oldState = q.front();
			q.pop();
			vi oldId = id(oldState);
			int& oldDir = direction[oldId];
 
			//--- Apply all applicable moves to it and handle the new state.��//���������õĶ���(ÿ������ת90.180.270)Ӧ�õ�������¼�µ�״̬��
			for (int move = 0; move<18; move++) {
				if (applicableMoves[phase] & (1 << move)) {							//��phase=2ʱ������FB��ֻ����ת180�㼴��Ⱥ��<U,D,F2,B2,L,R>
																					//��phase=3ʱ������FBLR��ֻ����ת180�㼴��Ⱥ��<U,D,F2,B2,L2,R2>
																					//��phase=4ʱ������UDFBLR��ֻ����ת180�㼴��Ⱥ��<U2,D2,F2,B2,L2,R2>
					//--- Apply the move.��//��ת��
					vi newState = applyMove(move, oldState);			//��ת���״̬
					vi newId = id(newState);							//��ת���״̬�������ͽǿ�ķ���
					int& newDir = direction[newId];						//ӵ���·����״̬�Ƿ���ֹ����ǣ��򷵻عؼ��ָ�newDir�������Թؼ���Ϊ0����direction
 
					//--- Have we seen this state (id) from the other direction already?��//�жϸ�״̬�Ƿ���ֹ���
					//--- I.e. have we found a connection?��//�ж��Ƿ��ܺ͹ؼ���Ϊ2��״̬��ϵ����������ܣ����ҵ��ⷨ���������������
					if (newDir  &&  newDir != oldDir) {								//��Ŀ��ħ����ת���״̬�ķ���ֵ������ħ��ĳһ״̬�ķ���ֵ���ʱif����
 
						//--- Make oldId represent the forwards and newId the backwards search state.��//oldId��ʾ֮ǰ��״̬�ķ���newId��ʾ��ת���״̬�ķ���ֵ�������ⷨ��
						if (oldDir > 1) {
							swap(newId, oldId);
							move = inverse(move);
						}
 
						//--- Reconstruct the connecting algorithm.��//������ϵ������״̬�Ĳ���move��
						vi algorithm(1, move);										//���ڴ�Ų���
						while (oldId != currentId) {								//��predecessor�������oldId==currentId������¼��Ҫ�Ĳ��赽algorithm���㷨��
																					//������ħ������algorithm�Ĳ�����ת�Ϳɵ���Ŀ��ħ����ת���״̬������ϵdirection�Ĺؼ���1��2��
							algorithm.insert(algorithm.begin(), lastMove[oldId]);
							oldId = predecessor[oldId];
						}
						while (newId != goalId) {									//��ԭ��Ŀ��ħ��״̬��Ҫת���Ĳ���
							algorithm.push_back(inverse(lastMove[newId]));
							newId = predecessor[newId];
						}
 
						//--- Print and apply the algorithm.��//��ӡ��Ӧ�ø��㷨��	serial_write(going_write[i]);
						for (int i = 0; i<(int)algorithm.size(); i++) {
							cout << "UDFBLR"[algorithm[i] / 3] << algorithm[i] % 3 + 1;		//��ӡ��Ҫ��ת����ͽǶȣ�1.2.3˳ʱ����ת90.180.270
							//serial_write(algorithm[i]);										//�����ִ����ת���棬�Լ��ҹ���
							answer.push_back(algorithm[i]);									//�ѽ������answer
 
							//serial_write("UDFBLR"[algorithm[i] / 3]);
							//serial_write(algorithm[i] % 3 + 1);
							currentState = applyMove(algorithm[i], currentState);			//��ת���ֵ����currentState����ǰֵ��
						}
 
						//--- Jump to the next phase.
						goto nextPhasePlease;				//����ţ�Ƶ���˹��˼Τ�ص���һ��
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
