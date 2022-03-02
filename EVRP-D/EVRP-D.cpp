#include<ilcplex\ilocplex.h>
#include<Windows.h>
#include<fstream>
#include<ilconcert/ilomodel.h>
#include<math.h>
#include<time.h>
#include <algorithm>
#include <random>
#define INF 0x7fffffff
#define rand(a,b) (rand()%(b-a+1)+a)
#define MAX_POINT 100
ILOSTLBEGIN
using namespace std;

typedef IloArray<IloFloatArray>Float2Array;
typedef IloArray<Float2Array>Float3Array;
typedef IloArray<IloIntArray>Int2Array;
typedef IloArray<Int2Array>Int3Array;

typedef IloArray<IloBoolVarArray>Bool2VarArray;
typedef IloArray<Bool2VarArray>Bool3VarArray;
typedef IloArray<Bool3VarArray>Bool4VarArray;
typedef IloArray<IloIntVarArray>Int2VarArray;
typedef IloArray<Int2VarArray>Int3VarArray;
typedef IloArray<IloFloatVarArray>Float2VarArray;
typedef IloArray<Float2VarArray>Float3VarArray;
//typedef IloArray<Float3VarArray>Float4VarArray;

typedef IloArray<IloNumVarArray>Num2VarArray;
typedef IloArray<Num2VarArray>Num3VarArray;

/*
	������Լ��������������˻����2
	����10
*/


int main()
{
	clock_t start, end;
	start = clock();
	IloEnv env;
	try {
		clock_t BeginTime = clock();
		IloModel model(env);		
		//��ȡ�ı��ĵ�
		ifstream Input("R9.txt");
		//���϶���
		int C = 9;//�ͻ�����C
		int Ct = 0;//�����ɿ�������
		int Cu = C - Ct;//�������˻��Կ�
		//int S = 2;//���վ����
		int D0 = 1;//�����ֿ�
		int D1 = 1;//���زֿ�
		int V0 = D0  + C;//�����㼯��
		int V1 = D0  + C + D1;//ȫ���㼯�ϣ�����㣩
		int v0 = D0  + Ct;//���˻����Է��ʵļ���


		//��������
		IloNum Bu = 10;//
		IloNum Bt = 20;
		IloNum p = 1.2;
		IloNum MAX = 99999999; //һ������������
		//���˻��Ϳ������ٶ�
		IloNum vu = 1.5;
		IloNum vt1 = 0.8;
		//IloNum vt1 = 0.5;

		IloNum wu = 1;//UAV self weight
		IloNum Qu = 3;//
		IloNum Qt = 20;//����������
		IloNum Su = 0;
		IloNum St = 0;
		//���������˻���λ�ɱ�
		IloNum CostT = 10;
		IloNum CostU = 0.1;

		//IloNum Z = 0;//1.5;//������λʱ�����ĵ���
		IloNum z = 0.2;//���˻���λʱ��ĵ���
		IloInt V = 4;//truck num

		//�������
		//Input >> Customer >> M;
		class point
		{
		public:
			IloInt ID;
			IloInt x;
			IloInt y;
			IloNum d;
			IloNum time;//��ʱ��
		};

		srand((int)time(NULL));
		point* Point = new point[V1];

		int i, j, k, m;
		//�����Ĳ���
		for (i = 0; i < V1; ++i)
		{
			Input >> Point[i].ID;
			Input >> Point[i].x;
			Input >> Point[i].y;
			Input >> Point[i].d;

		}
		for (i = 0; i < V1; ++i)
		{
			cout << Point[i].ID << "\t" << Point[i].d << endl;
		}
		
		//��λʱ��ĵ�������
		IloNum f = 1;
		//�������ٶ���0.6~0.9��Χ���������
		IloNum vt[100][100];
		default_random_engine e(time(0));//���������
		uniform_real_distribution<IloNum> rand_v(0.6, 0.9);//��Χ�������
		for (int i = 0; i != V1; ++i)
		{
			for (int j = 0; j != V1; ++j)
			{
				vt[i][j] = rand_v(e);
			}
		}
		//���������ľ���L_ij
		IloNumArray2 L(env, V1);
		for (int i = 0; i < V1; i++) {
			L[i] = IloNumArray(env, V1);
			for (int j = 0; j < V1; j++)
				L[i][j] = IloPower((IloPower((Point[i].x - Point[j].x), 2) + IloPower((Point[i].y - Point[j].y), 2)), 0.5);
		}

		//�������������˻�����ʱ��tu_ij
		IloNumArray2 tu(env, V1);
		for (int i = 0; i < V1; i++) {
			tu[i] = IloNumArray(env, V1);
			for (int j = 0; j < V1; j++)
				tu[i][j] = L[i][j] / vu;
		}

		//���������Ŀ�����ʻʱ��tt_ij
		IloNumArray2 tt(env, V1);
		for (int i = 0; i < V1; i++) {
			tt[i] = IloNumArray(env, V1);
			for (int j = 0; j < V1; j++)
				tt[i][j] = L[i][j] / vt1;
		}
		

		//���㿨�����ٵĵ�������
		/*IloNumArray2 E(env, V1);
		for (int i = 0; i < V1; i++)
		{
			E[i] = IloNumArray(env, V1);
			for (int j = 0; j < V1; j++)
			{
				E[i][j] = tt[i][j] * Z;
			}
		}*/

		int v;


		//------------------������� / ------------
		//���߱���zt[v][i]
		Bool2VarArray zt(env, V);
		for (v = 0; v < V; v++)
		{
			zt[v] = IloBoolVarArray(env, V1);
			for (i = 0; i < V1; i++)
			{
				zt[v][i] = IloBoolVar(env);
			}
		}


		//���߱���zu[i]
		Bool2VarArray zu(env, V);
		for (v = 0; v < V; v++)
		{
			zu[v] = IloBoolVarArray(env, V1);
			for (i = 0; i < V1; i++)
			{
				zu[v][i] = IloBoolVar(env);
			}
		}

		//�������߱���x[v][i][j]
		Bool3VarArray x(env, V + 1);
		for (v = 0; v < V; v++)
		{
			x[v] = Bool2VarArray(env, V1);
			for (i = 0; i < V1; i++)
			{
				x[v][i] = IloBoolVarArray(env, V1 + 1);
				for (j = 0; j < V1; j++)
				{
					x[v][i][j] = IloBoolVar(env);
				}
			}
		}


		//�������߱���y[v][i][j][k]
		Bool4VarArray y(env, V);
		for (v = 0; v < V; v++)
		{
			y[v] = Bool3VarArray(env, V1);
			for (i = 0; i < V1; ++i)
			{
				y[v][i] = Bool2VarArray(env, V1);
				for (j = 0; j < V1; ++j)
				{
					y[v][i][j] = IloBoolVarArray(env, V1 + 1);
					for (k = 0; k < V1; ++k)
					{
						y[v][i][j][k] = IloBoolVar(env);
					}
				}
			}
		}


		//��������t^AT[i]
		/*Float2VarArray t_AT(env, V1 + 1);
		for (i = 0; i < V1; ++i)
		{
			t_AT[i] = IloFloatVarArray(env, V);
			for (v = 0; v < V; ++v)
			{
				t_AT[i][v] = IloFloatVar(env);
			}
			
		}*/

		//��������t^LT[i]
		/*Float2VarArray t_LT(env, V1);
		for (i = 0; i < V1; ++i)
		{
			t_LT[i] = IloFloatVarArray(env, V);
			for (v = 0; v < V; ++v)
			{
				t_LT[i][v] = IloFloatVar(env);
			}

		}*/

		//��������YA[i]
		/*Float2VarArray YA(env, V);
		for (v = 0; v < V; v++)
		{
			YA[v] = IloFloatVarArray(env, V1);
			for (i = 0; i < V1; ++i)
			{
				YA[v][i] = IloFloatVar(env);
			}
		}*/


		//��������YL[i]
		/*Float2VarArray YL(env, V);
		for (v = 0; v < V; v++)
		{
			YL[v] = IloFloatVarArray(env, V1);
			for (i = 0; i < V1; ++i)
			{
				YL[v][i] = IloFloatVar(env);
			}
		}*/


		/*----------------||||||||||||||||||||||||||||||||-------Ŀ�꺯��-------||||||||||||||||||||||||||||-------------------*/
		IloExpr Cost(env);//Total
		IloExpr uCost(env);//UAV
		IloExpr tCost(env);//Truck
		IloExpr totalTime(env);//����ʻʱ��
		//Truck's cost

		for (i = 0; i < V0; i++)
		{
			for (j = D0; j < V1; j++)
			{
				for (v = 0; v < V; v++)
				{
					tCost += L[i][j] * x[v][i][j] * CostT;
					totalTime += (L[i][j] / vt1) * x[v][i][j];
				}
			}
		}


		//UAV's cost

		for (i = 0; i < V0; i++)
		{
			for (j = v0; j < V0; j++)
			{
				for (k = D0; k < V1; k++)
				{
					for (v = 0; v < V; v++)
					{
						uCost += (L[i][j] + L[j][k]) * y[v][i][j][k] * CostU;
					}
				}
			}
		}

		Cost = uCost + tCost;
		model.add(IloMinimize(env, Cost));

		/*----------------||||||||||||||||||||||||||||||||-------����Լ��-------||||||||||||||||||||||||||||-------------------*/
		//2��ͬ����Ŀͻ�
		for (j = v0; j <= V0; j++)
		{
			IloExpr e1(env);
			IloExpr e2(env);
			for (v = 0; v < V; v++)
			{
				e1 += zt[v][j];
				e2 += zu[v][j];
			}
			model.add(e1 + e2 == 1);
			e1.end();
			e2.end();
		}

		

		//4���ڳ��վ����ȥ�ɲ�ȥ���ӵ������Ķ���
		
		/*for (v = 0; v < V; v++)
		{
			for (k = 0; k <= V0; ++k)
			{
				for (j = D0; j < D0 + S; j++)
				{
					model.add(x[v][j][k] <= 1);
					model.add(x[v][k][j] <= 1);
				}
			}
		}*/
		

		//6������ƽ��
		for (v = 0; v < V; v++)
		{
			for (j = D0; j < V0; j++)
			{
				IloExpr e1(env);
				for (i = 0; i < V0; i++)
				{
					e1 += x[v][i][j];
				}
				IloExpr e2(env);
				for (k = D0; k < V1; k++)
				{
					e2 += x[v][j][k];
				}
				model.add(e1 == e2);
				e1.end();
				e2.end();
			}
		}
		//5���Ÿ������ĵ㿨������ȥ��
		for (v = 0; v < V; v++)
		{
			for (j = D0; j < V0; j++)
			{
				IloExpr e(env);
				IloExpr e1(env);
				for (i = 0; i < V0; i++)
				{
					e += x[v][i][j];
					
				}
				
				model.add(e  == zt[v][j]);
				e.end();
				e1.end();
			}
		}
		//7���Ÿ����˻��Ŀͻ����˻��������
		for (v = 0; v < V; v++)
		{
			for (j = v0; j < V0; j++)
			{
				IloExpr e(env);
				for (i = 0; i < V0; i++)
				{
					for (k = D0; k < V1; k++)
					{
						e += y[v][i][j][k];
					}
				}
				model.add(zu[v][j] == e);
				e.end();
			}
		}
		//8���˻�����ɽ�����ǿ�������������������
		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						model.add(x[v][i][k] >= y[v][i][j][k]);
					}
				}
			}
		}
		//��ֹ�ӳ��վ���
		/*for (v = 0; v < V; v++)
		{
			for (i = D0; i < D0 + S; i++)
			{
				IloExpr e(env),e1(env);
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						e += y[v][i][j][k];
						
						model.add(e == 0);
					}
					for (int g = 0; g < V0; g++)
					{
						e1 += y[v][g][j][i];
						model.add(e1 == 0);
					}
				}
				e.end(), e1.end();
			}
		}*/
		//9ÿ���������ܴӳ��վֱ�ӵ����վ
		/*for (i = D0; i < D0 + S - 1; i++)
		{
			for (i = D0; i < D0 + S - 1; i++)
			{
				model.add(x[v][i][j] == 0);
			}
		}*/
		
		//10ÿ���������Ӳֿ����
		for (v = 0; v < V; v++)
		{
			IloExpr e1(env);
			for (j = D0; j < V0; j++)
			{
				e1 += x[v][0][j];
			}
			model.add(e1 <= 1);
			e1.end();
		}

		//11ÿ������������ֿ�
		for (v = 0; v < V; v++)
		{
			IloExpr e1(env);
			for (j = D0; j < V0; j++)
			{
				e1 += x[v][j][V1];
			}
			model.add(e1 <= 1);
			e1.end();
		}
		
		//12ĳ��ֻ�����һ��
		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				IloExpr e(env);
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						e += y[v][i][j][k];
					}
				}
				model.add(e <= 1);
				e.end();
			}
		}
		
		//13ĳ��ֻ�ܽ���һ��
		for (v = 0; v < V; v++)
		{
			for (k = D0; k < V1; k++)
			{
				IloExpr e(env);
				for (j = v0; j < V0; j++)
				{
					for (i = 0; i < V0; i++)
					{
						e += y[v][i][j][k];
					}
				}
				model.add(e <= 1);
				e.end();
			}
		}

		//ͬһ����ֻ����ɽ���geһ��
		/*for (v = 0; v < V; v++)
		{
			for (i = D0; i < V0; i++)
			{
				IloExpr e1(env);
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						e1 += y[v][i][j][k];
					}
				}
				IloExpr e2(env);
				for (j = 0; j < V0; j++)
				{
					for (k = v0; k < V0; k++)
					{
						e2 += y[v][j][k][i];
					}
				}
				model.add(e1 + e2 <= 2);
				e1.end();
				e2.end();
			}
		}*/


		//����ÿ����ĵ���������ڵ���0
		/*for (v = 0; v < V; v++)
		{
			for (i = D0; i < V1; i++)
			{
				model.add(YA[v][i] >= 0);
			}
		}*/

		//11����㿨��ʣ�����
		/*for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				for (j = D0; j < V1; j++)
				{
					if (i != j)
					{
						model.add(YA[v][j] <= YL[v][i] - Z * L[i][j] * x[v][i][j] + Bt * (1 - x[v][i][j]));
						model.add(YA[v][j] >= YL[v][i] - Z * L[i][j] * x[v][i][j] - Bt * (1 - x[v][i][j]));
					}
				}
			}
		}*/
		/*for (v = 0; v < V; v++)
		{
			for (i = 0; i < V1; i++)
			{
				model.add(YL[v][i] <= Bt);
			}
		}*/
		//12����㵽����뿪��ʣ��������
		/*for (v = 0; v < V; v++)
		{
			for (i = D0 + S; i < V1; i++)
			{
				model.add(YA[v][i] == YL[v][i]);
			}
		}*/
		
		//�ֿ����ʱ����
		/*for (v = 0; v < V; v++)
		{
			model.add(YL[v][0] == Bt);
			model.add(YA[v][0] == YL[v][0]);
		}*/

		//14���վ����Լ��
		//for (v = 0; v < V; v++)
		//{
		//	for (i = D0; i < D0 + S; i++)
		//	{
		//		model.add(YA[v][i] <= Bt);// YL[v][i]);
		//		model.add(YL[v][i] == Bt);
		//	}
		//}

		//15���˻����������Լ��
		for (v = 0; v < V; v++)
		{
			for (j = v0; j < V0; j++)
			{
				IloExpr e(env);
				for (i = 0; i < V0; i++)
				{
					for (k = D0; k < V1; k++)
					{
						e += y[v][i][j][k];
					}
				}
				model.add(zu[v][j] * Point[j].d <= Qu);
				e.end();
			}
		}

		//����·������Լ�����������˻����������
		for (v = 0; v < V; ++v)
		{
			IloExpr e(env);
			for (i = D0; i < V1; ++i)
			{
				e += zt[v][i] * Point[i].d + zu[v][i] * Point[i].d;
			}
			model.add(e <= Qt);
			e.end();
		}

	

		//truck subroute eliminate
		IloIntVarArray ui(env, V1);
		for (i = 0; i < V1; ++i)
		{
			ui[i] = IloIntVar(env);
		}

		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V1; ++i)
			{
				for (j = 0; j < V1; ++j)
				{
					model.add(ui[i] - ui[j] + (V1)*x[v][i][j] <= V1 - 2);
				}
			}
		}

		for (v = 0; v < V; v++)
		{
			IloExpr LeftSum(env), R(env);
			for (i = 0; i < V0; i++)
				for (j = D0; j < V1; j++)
				{
					LeftSum += x[v][i][j];
				}
			for (i = D0; i < V0; i++)
			{
				R += zt[v][i];
			}
			model.add(LeftSum <= (1 - (1 / MAX)) * R + 1);

		}

		

		//���˻�����ʱ��Լ��
		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						model.add(tu[i][j] + Su + tu[j][k] <= tt[i][k] + MAX * (1 - y[v][i][j][k]));
						//model.add(tu[i][j] + Su + tu[j][k] <= tt[i][k] + MAX * (2 - y[v][i][j][k] - x[v][i][k]));

					}
				}
			}
		}

		//���˻����˵���Լ��
		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						model.add(z * (tu[i][j] + Su) * (wu )//+ Point[j].d)
							+ z * (tt[i][k] - tu[i][j] - Su) * wu <= Bu + MAX * (1 - y[v][i][j][k]));

						/*model.add(z * (tu[i][j] + Su) * (wu+ Point[j].d)
							+z * (tt[i][k] - tu[i][j] - Su) * wu <= Bu + MAX * (1 - y[v][i][j][k]));*/

						//model.add(z* (tu[i][j] + Su)* (wu + Point[j].d)
							//+ z * (tt[i][k] - tu[i][j] - Su) * wu <= Bu + MAX * (2 - y[v][i][j][k] - x[v][i][k]));
					}//tt[i][k] - tu[i][j] - Su
				}
			}
		}



		IloCplex cplex(model);
		//cplex.setOut(env.getNullStream());//�����Ƿ���dos���������������Ϣ
		//cplex.setParam(IloCplex::TiLim, 10);
		if (cplex.solve())
		{

			cout << "��С·���ɱ���" << cplex.getObjValue() << endl;
			cout << "·���Լ������仯��" << endl;
			for (v = 0; v < V; v++)
			{
				for (i = 0; i < V0; i++)
				{
					for (j = D0; j < V1; j++)
					{
						if (cplex.getValue(x[v][i][j]) > 0.9)
						{

							cout << v << ":" << i << "--->" << j << endl;
							//cout << cplex.getValue(YA[v][i])<<" "<<cplex.getValue(YL[v][i]) << "--->" << cplex.getValue(YA[v][j]) << " " << cplex.getValue(YL[v][j]) << endl;
							//cout << "                      " << endl;
						}
					}
				}
			}

			cout << "���˻�·��" << endl;
			for (v = 0; v < V; v++)
			{
				for (i = 0; i < V0; i++)
				{
					for (j = v0; j < V0; j++)
					{
						for (k = D0; k < V1; k++)
						{
							if (cplex.getValue(y[v][i][j][k]) > 0.9)
							{
								cout << v << "(" << i << "--->" << j << "--->" << k << ")" << endl;
							}
						}
					}
				}
			}

			
		}
		else {
			cout << " No solution found" << endl;
		}
		end = clock();
		double time = double(end - start) / CLOCKS_PER_SEC;
		cout << "�ܼ���ʱ " << time << "��" << endl;
		system("pause");
		ofstream out;

		out.open("./R11_result.txt");
		out << "���Ž�ֵΪ���� " << cplex.getObjValue() << "��" << endl;
		out << "����·�ߣ�" << endl;
		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				for (j = D0; j < V1; j++)
				{
					if (cplex.getValue(x[v][i][j]) > 0.9)
					{

						out << v << ":" << i << "--->" << j << endl;
						//cout << cplex.getValue(YA[v][i])<<" "<<cplex.getValue(YL[v][i]) << "--->" << cplex.getValue(YA[v][j]) << " " << cplex.getValue(YL[v][j]) << endl;
						//cout << "                      " << endl;
					}
				}
			}
		}
		out << "���˻�·��" << endl;
		for (v = 0; v < V; v++)
		{
			for (i = 0; i < V0; i++)
			{
				for (j = v0; j < V0; j++)
				{
					for (k = D0; k < V1; k++)
					{
						if (cplex.getValue(y[v][i][j][k]) > 0.9)
						{
							out << v << "(" << i << "--->" << j << "--->" << k << ")" << endl;
						}
					}
				}
			}
		}
		
		out.close();
	}
	catch (IloException& ex) {
		cerr << "Error: " << ex << endl;
	}
	catch (...) {
		cerr << "Error" << endl;
	}
	env.end();

	
	return 0;
}
