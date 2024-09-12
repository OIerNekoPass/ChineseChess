#include <graphics.h>
#include <bits/stdc++.h>
#define Pos pair<int,int>
#define IT deque<string>::iterator
#define  KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)   
#define  KEY_UP(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 0:1)  
using namespace std;
PIMAGE sel_pic,chess_pic[2][10],board_pic,side_box,checkmate_pic,regret_pic,win_pic,lop_pic;
PIMAGE Red_pic,Black_pic;
color_t transColor = EGERGB(0xCC, 0x6E, 0x00);
bool Player;
//color_t transColor = EGERGB(0xFF, 0xFF, 0xFF);
const int Col=9,Row=10;
int Max_dep=4;
const int Mail_Size=5;
int OP_Round=0;
struct Operate_Log{
	int st_cid,ed_cid;
	Pos st_pos,ed_pos;
	Operate_Log(int sc=0,int ec=0,Pos sp=Pos(-1,-1),Pos ep=Pos(-1,-1)):st_cid(sc),ed_cid(ec),st_pos(sp),ed_pos(ep){};
}OP_Log[2][10000];

void Print_Button(bool status){
	putimage_transparent(NULL,regret_pic,593,441,EGERGB(0x8A,0x75,0x56)); //EGERGB(0x81,0x69,0x4C)
	setcolor(status?EGERGB(0xE1,0x00,0x00):BLACK);
	xyprintf(645,463,"悔棋");
}

struct Mail_Box{
	deque<string> q;
	deque<int> color;
	int size;
	Mail_Box(){
		size=0;	
	}
	void add_mail(string s,int c){
		if(size==Mail_Size) q.pop_front(),color.pop_front();
		else size++;
		q.push_back(s);color.push_back(c);
	}
	void print_mail(){
		int line=20;
		putimage(540,0,side_box);Print_Button(0);
		deque<int>::iterator itc=color.begin();
		for(IT it=q.begin();it!=q.end();it++,itc++){
			string s=*it;
			int c=*itc;
			setcolor(c?BLACK:EGERGB(0xE1,0x00,0x00));
			xyprintf(595,line,s.c_str());
			line+=50;
		}
	}
}Mail;


//cid   0:帅    1~2:车     3~4:马     5~6:象     7~8:士     9~10:炮     11~15:兵  for Red
//+20后表示Black

int mp[20][20];

//0帅 1车 2马 3象 4士 5炮 6兵
//兵过河价值+10
int Ori_Val[]={10000000,9500,400,200,150,550,130};
int Mid_Val[]={10000000,9500,500,200,200,500,130};
int End_Val[]={10000000,9500,550,250,200,500,130};
int Chess_Val[10];
//行动力价值 
int Move_Val[]={0,7,13,1,1,7,2};

int Soldier_Pos[10][9]={
{1,3,9,10,12,10,9,3,1},
{18,36,56,95,118,95,56,36,18},
{15,28,42,73,80,73,42,28,15},
{13,22,30,42,50,42,30,22,13},
{8,17,28,21,26,21,28,17,8},
{3,0,8,0,8,0,8,0,3},
{-1,0,-3,0,5,0,-3,0,-1},
{0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0},
};
int Car_Pos[10][9]={
{185,195,190,210,220,210,190,195,185},
{185,203,198,230,245,230,198,203,185},
{180,198,190,215,225,215,190,198,180},
{180,200,195,220,230,220,195,200,180},
{180,190,180,205,225,205,180,190,180},
{155,185,172,215,215,215,172,185,155},
{110,148,135,185,190,185,135,148,110},
{100,115,105,140,135,140,105,115,100},
{115,95,100,155,155,155,100,95,115},
{20,120,105,140,115,140,105,120,20},
};

int Horse_Pos[10][9]={
{80,105,135,120,80,120,135,105,80},
{80,115,200,135,105,135,200,115,80},
{120,125,135,150,145,150,135,125,120},
{105,175,145,150,175,145,150,175,105},
{90,135,125,145,135,145,125,135,90},
{80,120,135,125,120,125,135,120,80},
{45,90,105,90,110,90,105,90,45},
{80,45,105,105,80,105,105,45,80},
{20,45,80,80,-10,80,80,45,20},
{20,-20,20,20,20,20,20,-20,20},
};

int Cannon_Pos[10][9]={
{50,40,30,0,0,0,30,40,50},
{0,0,0,0,0,0,0,0,0},
{0,0,0,0,40,0,0,0,0},
{0,0,0,0,40,0,0,0,0},
{0,0,0,0,50,0,0,0,0},
{0,0,0,0,40,0,0,0,0},
{0,0,0,0,50,0,0,0,0},
{0,0,10,0,40,0,10,0,0},
{0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0},
};

int Belong(int cid){
	if(cid<0) return -1;//empty
	if(cid<20) return 0;//Red
	return 1;//Black
}
int Type(int cid){
	if(cid>=20) cid-=20;
	if(cid==0) return 0;
	if(cid<11) return (cid+1)/2;
	return 6;
}
void Load_Pic(){
	sel_pic = newimage();
	board_pic = newimage();
	side_box = newimage();
	checkmate_pic = newimage();
	regret_pic = newimage();
	win_pic = newimage();
	lop_pic = newimage();
	Red_pic = newimage();
	Black_pic = newimage();
	for(int w=0;w<2;++w)
	for(int i=0;i<7;++i) chess_pic[w][i]=newimage();
	
	getimage(sel_pic,"pic/select.png");
	getimage(board_pic,"pic/board.png");
	getimage(side_box,"pic/sidebox.png");
	getimage(checkmate_pic,"pic/checkmate.png");
	getimage(regret_pic,"pic/regret.png");
	getimage(win_pic,"pic/win.png");
	getimage(lop_pic,"pic/lop.png");
	getimage(Red_pic,"pic/red/ch.png");
	getimage(Black_pic,"pic/black/ch.png");
	//red
	getimage(chess_pic[0][0],"pic/red/0帅.png");
	getimage(chess_pic[0][1],"pic/red/0车.png");
	getimage(chess_pic[0][2],"pic/red/0马.png");
	getimage(chess_pic[0][3],"pic/red/0相.png");
	getimage(chess_pic[0][4],"pic/red/0仕.png");
	getimage(chess_pic[0][5],"pic/red/0炮.png");
	getimage(chess_pic[0][6],"pic/red/0兵.png");
	//black
	getimage(chess_pic[1][0],"pic/black/1将.png");
	getimage(chess_pic[1][1],"pic/black/1车.png");
	getimage(chess_pic[1][2],"pic/black/1马.png");
	getimage(chess_pic[1][3],"pic/black/1象.png"); 
	getimage(chess_pic[1][4],"pic/black/1士.png");
	getimage(chess_pic[1][5],"pic/black/1炮.png");
	getimage(chess_pic[1][6],"pic/black/1卒.png");
}

void Set_Chess(){
//	for(int i=0;i<9;++i)
	/*ifstream fin;
	fin.open("board.txt");
	for(int i=0;i<=9;++i)
	for(int j=0;j<=8;++j)
	fin>>mp[i][j];
	
	for(int i=0;i<=9;++i,puts(""))
	for(int j=0;j<=8;++j)
	cout<<mp[i][j]<<" ";
	fin.close();
	return ;*/
	
	for(int i=0;i<=15;++i)
	for(int j=0;j<=15;++j)
	mp[i][j]=-1;
	
	//black
	mp[0][0]=21;mp[0][1]=23;mp[0][2]=25;mp[0][3]=27;mp[0][4]=20;mp[0][5]=28;mp[0][6]=26;mp[0][7]=24;mp[0][8]=22;
	mp[2][1]=29;mp[2][7]=30;
	mp[3][0]=31;mp[3][2]=32;mp[3][4]=33;mp[3][6]=34;mp[3][8]=35;
	//red
	mp[6][0]=11;mp[6][2]=12;mp[6][4]=13;mp[6][6]=14;mp[6][8]=15;
	mp[7][1]=9;mp[7][7]=10;
	mp[9][0]=1;mp[9][1]=3;mp[9][2]=5;mp[9][3]=7;mp[9][4]=0;mp[9][5]=8;mp[9][6]=6;mp[9][7]=4;mp[9][8]=2;
	
	return ;
}
void Print_Chess(int X,int Y,int cid){
	X*=60;Y*=60;
	putimage_transparent(NULL,chess_pic[Belong(cid)][Type(cid)],X,Y,transColor);
}
void Print_Board(){
	putimage(0,0,board_pic);
	Pos st,ed;
	if(OP_Round){
		if(Player==0) st=OP_Log[1][OP_Round-1].st_pos,ed=OP_Log[1][OP_Round-1].ed_pos;
		else st=OP_Log[0][OP_Round].st_pos,ed=OP_Log[0][OP_Round].ed_pos;
		putimage_transparent(NULL,lop_pic,st.second*60,st.first*60,transColor);
		putimage_transparent(NULL,lop_pic,ed.second*60,ed.first*60,transColor);
	}
	for(int i=0;i<Row;++i) 
	for(int j=0;j<Col;++j){
		if(mp[i][j]==-1) continue;
		Print_Chess(j,i,mp[i][j]);
	}
	Mail.print_mail();
}
void Print_Select(int r,int c){
	if(r<0||c<0||r>9||c>8) return ;
	putimage_transparent(NULL,sel_pic,c*60,r*60,transColor);
}

bool Move_Map[20][20];
Pos Player_Move(){
	int MX,MY,r,c;
	bool push=0,sel=0;
	Pos pos(0,0);
	Print_Board();
	for(int i=0;i<=9;++i)
	for(int j=0;j<=8;++j)
	if(Move_Map[i][j]) Print_Select(i,j);
	
	while(1){
		mousepos(&MX,&MY);     //获取鼠标当前坐标 
		if(KEY_DOWN(VK_LBUTTON)) push=1;
		if(push&&KEY_DOWN(VK_LBUTTON)){
			c=MX/60;r=MY/60;  //转成棋盘坐标 
			if(c>9) c=-1;
			if(r>10) r=-1;
			if(r==-1 || c==-1) return Pos(-1,-1);
			if(Move_Map[r][c]) return Pos(r,c);
			else return Pos(-1,-1);
		}
		Sleep(100);
	}
	return pos;
}
int Boss_X[]={-1,1,0,0},Boss_Y[]={0,0,-1,1};
int Horse_X[]={1,1,-1,-1,2,2,-2,-2},Horse_Y[]={2,-2,2,-2,1,-1,1,-1};
int Feet_X[]={0,0,0,0,1,1,-1,-1},Feet_Y[]={1,-1,1,-1,0,0,0,0};
int Elephant_X[]={2,2,-2,-2},Elephant_Y[]={2,-2,2,-2};
//int Boss_X[]={},Boss_Y[]={};
int Guard_X[]={1,1,-1,-1},Guard_Y[]={1,-1,1,-1};
//int Boss_X[]={},Boss_Y[]={};
//int Boss_X[]={},Boss_Y[]={};
//int Boss_X[]={},Boss_Y[]={};

bool Judge_Pos(Pos A){
	if(A.first>=0&&A.first<=9&&A.second>=0&&A.second<=8) return true;
	return false;
}
int Print_Move(int cid,Pos pos){
	int r,c,hr,hc,dir,cnt=0;
	for(int i=0;i<10;++i)
	for(int j=0;j<10;++j) Move_Map[i][j]=0;
	
	switch(Type(cid)){
		case 0:{//帅or将 
			for(int w=0;w<4;w++){
				r=pos.first+Boss_X[w];c=pos.second+Boss_Y[w];
				if(Belong(cid)==0){//红方 
					if(r>=7&&r<=9&&c>=3&&c<=5&&Belong(mp[r][c])!=0)
					Move_Map[r][c]=1,cnt++;
				}
				else{//黑方 
					if(r>=0&&r<=2&&c>=3&&c<=5&&Belong(mp[r][c])!=1)
					Move_Map[r][c]=1,cnt++;
				}
			}
			
			if(Belong(cid)==0){//红方飞帅 
				r=pos.first-1;c=pos.second;
				while(r>=0&&mp[r][c]==-1) r--;
				if(mp[r][c]==20) Move_Map[r][c]=1,cnt++;
			}
			else{//黑方飞将 
				r=pos.first+1;c=pos.second;
				while(r<=9&&mp[r][c]==-1) r++;
				if(mp[r][c]==0) Move_Map[r][c]=1,cnt++;
			}
			break;
		}
		case 1:{//车 
			r=pos.first;c=pos.second;
			r++;while(r<=9&&mp[r][c]==-1) Move_Map[r][c]=1,r++,cnt++;
			if(r<=9&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			
			r=pos.first;c=pos.second;
			r--;while(r>=0&&mp[r][c]==-1) Move_Map[r][c]=1,r--,cnt++;
			if(r>=0&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			
			r=pos.first;c=pos.second;
			c++;while(c<=8&&mp[r][c]==-1) Move_Map[r][c]=1,c++,cnt++;
			if(c<=8&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			
			r=pos.first;c=pos.second;
			c--;while(c>=0&&mp[r][c]==-1) Move_Map[r][c]=1,c--,cnt++;
			if(c>=0&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			break;
		}
		case 2:{//马 
			for(int w=0;w<8;w++){
				r=pos.first+Horse_X[w];c=pos.second+Horse_Y[w];
				if(Judge_Pos(Pos(r,c))){
					hr=pos.first+Feet_X[w];hc=pos.second+Feet_Y[w];
					if(mp[hr][hc]==-1 && Belong(mp[r][c])!=Belong(cid))
					Move_Map[r][c]=1,cnt++;
				}
			}
			break;
		}
		case 3:{//象 
			for(int w=0;w<4;w++){
				r=pos.first+Elephant_X[w];c=pos.second+Elephant_Y[w];
				if(Judge_Pos(Pos(r,c))){
					if(Belong(cid)==0&&r<=4) continue;
					if(Belong(cid)==1&&r>=5) continue;
					hr=pos.first+Elephant_X[w]/2;hc=pos.second+Elephant_Y[w]/2;
					if(mp[hr][hc]==-1 && Belong(mp[r][c])!=Belong(cid))
					Move_Map[r][c]=1,cnt++;
				}
			}
			break;
		}
		case 4:{//士 
			for(int w=0;w<4;w++){
				r=pos.first+Guard_X[w];c=pos.second+Guard_Y[w];
				if(Belong(cid)==0){//红方 
					if(r>=7&&r<=9&&c>=3&&c<=5&&Belong(mp[r][c])!=0)
					Move_Map[r][c]=1,cnt++;
				}
				else{//黑方 
					if(r>=0&&r<=2&&c>=3&&c<=5&&Belong(mp[r][c])!=1)
					Move_Map[r][c]=1,cnt++;
				}
			}
			break;
		}
		case 5:{//炮 
			r=pos.first;c=pos.second;
			r++;while(r<=9&&mp[r][c]==-1) Move_Map[r][c]=1,r++,cnt++;
			if(r<9){
				r++;while(r<=9&&mp[r][c]==-1) r++;
				if(r<=9&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			}
			
			r=pos.first;c=pos.second;
			r--;while(r>=0&&mp[r][c]==-1) Move_Map[r][c]=1,r--,cnt++;
			if(r>0){
				r--;while(r>=0&&mp[r][c]==-1) r--;
				if(r>=0&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			}
			
			r=pos.first;c=pos.second;
			c++;while(c<=8&&mp[r][c]==-1) Move_Map[r][c]=1,c++,cnt++;
			if(c<8){
				c++;while(c<=8&&mp[r][c]==-1) c++;
				if(c<=8&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			}
			
			r=pos.first;c=pos.second;
			c--;while(c>=0&&mp[r][c]==-1) Move_Map[r][c]=1,c--,cnt++;
			if(c>0){
				c--;while(c>=0&&mp[r][c]==-1) c--;
				if(c>=0&&Belong(mp[r][c])!=Belong(cid)) Move_Map[r][c]=1,cnt++;
			}
			break;
		}
		case 6:{//兵or卒 
			if(Belong(cid)==0) dir=-1;
			else dir=1;
			r=pos.first;c=pos.second;
			if(Judge_Pos(Pos(r+dir,c))&&Belong(mp[r+dir][c])!=Belong(cid)) Move_Map[r+dir][c]=1,cnt++;
			if(Belong(cid)==0&&r<=4){
				if(Judge_Pos(Pos(r,c-1))&&Belong(mp[r][c-1])!=Belong(cid))
				Move_Map[r][c-1]=1,cnt++;
				if(Judge_Pos(Pos(r,c+1))&&Belong(mp[r][c+1])!=Belong(cid))
				Move_Map[r][c+1]=1,cnt++;
			}
			else if(Belong(cid)==1&&r>=5){
				if(Judge_Pos(Pos(r,c-1))&&Belong(mp[r][c-1])!=Belong(cid))
				Move_Map[r][c-1]=1,cnt++;
				if(Judge_Pos(Pos(r,c+1))&&Belong(mp[r][c+1])!=Belong(cid))
				Move_Map[r][c+1]=1,cnt++;
			}
			break;
		}
	}
	return cnt;
}

Pos Player_Operate(bool side){
	int MX,MY;
	int Row=-1,Col=-1;
	int r,c;
	bool push=0,sel=0;
	Print_Board();Print_Button(0);
	while(1){
		mousepos(&MX,&MY);
		if(KEY_DOWN(VK_LBUTTON)) push=1;
		if(push&&KEY_UP(VK_LBUTTON)) sel=1;
		
		if(MX>=630&&MX<=750&&MY>=450&&MY<=520) r=c=99;
		else{
			c=MX/60;r=MY/60;
			if(c>8) c=-1;
			if(r>9) r=-1;
		}
		if(r!=Row||c!=Col){
			Print_Board();Print_Button(0);
			if(r==99&&c==99&&push==1) Print_Button(1);
			else Print_Select(r,c);
		}
		if(sel==1){
			if(r==99&&c==99){
				Print_Button(0);
				return Pos(r,c);
			}
			if(r>=0&&c>=0&&mp[r][c]!=-1&&Belong(mp[r][c])==side&&Print_Move(mp[r][c],Pos(r,c))>0)
			return Pos(r,c);
			else push=0,sel=0;
		}
		Sleep(100);
//		cout<<r<<","<<c<<endl;
	}
}

int Judge_Val(int side){
	int be[3];
	be[0]=(side?-1:1);be[1]=(side?1:-1);
	int cnt=0,cid;
	long long ans=0;
	for(int i=0;i<=9;++i)
	for(int j=0;j<=8;++j)
	if(mp[i][j]!=-1){
		cid=mp[i][j];
		
		//棋子价值
		ans+=be[Belong(cid)]*Chess_Val[Type(cid)];
		
		//行动力价值 
		cnt=Print_Move(cid,Pos(i,j));
		ans+=cnt*be[Belong(cid)]*Move_Val[Type(cid)];
		
		//位置价值
		if(Type(cid)==6){//兵 
			if(Belong(cid)==0) ans+=Soldier_Pos[i][j]*be[0];
			else ans+=Soldier_Pos[9-i][j]*be[1];
		}
		else if(Type(cid)==1){//车 
			if(Belong(cid)==0) ans+=Car_Pos[i][j]*be[0];
			else ans+=Car_Pos[9-i][j]*be[1];
		}
		else if(Type(cid)==2){//马 
			if(Belong(cid)==0) ans+=Horse_Pos[i][j]*be[0];
			else ans+=Horse_Pos[9-i][j]*be[1];
		}
		else if(Type(cid)==5){//炮 
			if(Belong(cid)==0) ans+=Cannon_Pos[i][j]*be[0];
			else ans+=Cannon_Pos[9-i][j]*be[1];
		}
	}
	
	return ans;
}
bool Checkmate(int side,bool jmp=0){
	int x,y;
	if(side==0){ //检查红方是否将军黑方 
		for(int i=0;i<=9;++i)
		for(int j=0;j<=8;++j)
		if(mp[i][j]==20) x=i,y=j;
		
		for(int i=0;i<=9;++i)
		for(int j=0;j<=8;++j)
		if(mp[i][j]!=-1&&Belong(mp[i][j])==0){
			Print_Move(mp[i][j],Pos(i,j));
			if(Move_Map[x][y]){
				if(!jmp) putimage_transparent(NULL,checkmate_pic,125,150,transColor);
				return 1;
			}
		}
	}
	else{ //检查黑方是否将军红方 
		for(int i=0;i<=9;++i)
		for(int j=0;j<=8;++j)
		if(mp[i][j]==0) x=i,y=j;
		
		for(int i=0;i<=9;++i)
		for(int j=0;j<=8;++j)
		if(mp[i][j]!=-1&&Belong(mp[i][j])==1){
			Print_Move(mp[i][j],Pos(i,j));
			if(Move_Map[x][y]){
				if(!jmp) putimage_transparent(NULL,checkmate_pic,125,150,transColor);
				return 1;
			}
		}
	}
	return 0;
}
bool Judge_Over(int side){//检查side方是否被将死 
	int tmp1,tmp2;
	int tmp_move[20][20];
	for(int i=0;i<=9;++i)
	for(int j=0;j<=8;++j)
	if(Belong(mp[i][j])==side){
		Print_Move(mp[i][j],Pos(i,j));
		for(int r=0;r<=10;++r)
		for(int c=0;c<=10;++c)
		tmp_move[r][c]=Move_Map[r][c];
		for(int r=0;r<=9;++r)
		for(int c=0;c<=8;++c)
		if(tmp_move[r][c]){
			tmp1=mp[i][j];tmp2=mp[r][c];
			mp[r][c]=mp[i][j];mp[i][j]=-1;
			if(Checkmate(side^1,1)==0){
				mp[i][j]=tmp1;mp[r][c]=tmp2;
//				printf("%d=(%d,%d) to %d=(%d,%d) can alive!\n",tmp1,i,j,tmp2,r,c);
				return 0;//未被将死 
			}
			mp[i][j]=tmp1;mp[r][c]=tmp2;
		}
	}
	return 1;//被将死 
}
struct Max_Status{
	Pos stp,edp;
	int val;
	Max_Status(Pos u=Pos(-1,-1),Pos v=Pos(-1,-1),int x=-100000000):stp(u),edp(v),val(x){}
	bool operator < (const Max_Status &o) const{
		return val<o.val;
	}
};
struct Min_Status{
	Pos stp,edp;
	int val;
	Min_Status(Pos u=Pos(-1,-1),Pos v=Pos(-1,-1),int x=-100000000):stp(u),edp(v),val(x){}
	bool operator < (const Min_Status &o) const{
		return val>o.val;
	}
};
string Move_Info(int cid,Pos st,Pos ed){
	string s="",num[]={"一","二","三","四","五","六","七","八","九"};
	string Type_R[]={"帅","车","马","相","仕","炮","兵"},Type_B[]={"将","车","马","象","士","炮","卒"};
	int type=Type(cid),r1=st.first,r2=ed.first,c1=st.second,c2=ed.second;
	if(type<=1||type>=5){//帅,车,炮,兵
		if(Belong(cid)==0){
			s+=Type_R[type];s+=num[8-c1];
			if(r1==r2) s+="平",s+=num[8-c2];
			else if(r1<r2) s+="退",s+=num[r2-r1-1];
			else s+="进",s+=num[r1-r2-1];
		}
		else{
			s+=Type_B[type];s+=num[c1];
			if(r1==r2) s+="平",s+=num[c2];
			else if(r1<r2) s+="进",s+=num[r2-r1-1];
			else s+="退",s+=num[r1-r2-1];
		}
	}
	else{//马,相,仕
		if(Belong(cid)==0){
			s+=Type_R[type];s+=num[8-c1];
			if(r1<r2) s+="退",s+=num[8-c2];
			else s+="进",s+=num[8-c2];
		}
		else{
			s+=Type_B[type];s+=num[c1];
			if(r1<r2) s+="进",s+=num[c2];
			else s+="退",s+=num[c2];
		}
	}
	return s;
}

int check_black,check_red;
int dfs(int side,int dep,int turn,int best_ans=-40000000,int worst_ans=40000000){
//	cout<<(side?"black":"red")<<" dep="<<dep<<" turn="<<(turn?"b":"r")<<" ans="<<best_ans<<"\n";
//	cout<<dep<<"!\n";
	if(dep==0){
		int cnt=0;
		for(int i=0;i<=9;++i)
		for(int j=0;j<=8;++j)
		if(mp[i][j]!=-1) cnt++;
		
		if(cnt>=27&&OP_Round<=10) 
		for(int i=0;i<7;++i) Chess_Val[i]=Ori_Val[i];
		else if(cnt>=18&&OP_Round<=40) 
		for(int i=0;i<7;++i) Chess_Val[i]=Mid_Val[i];
		else for(int i=0;i<7;++i) Chess_Val[i]=End_Val[i];
	}
	int val=Judge_Val(side);
//	Print_Board();Sleep(100);
	//如果 对方被将死 / 己方被将死 / 超过深搜最大层 
	if(side==turn&&Judge_Over(side)) return -15000000;//如果自己被将死 
	if(side!=turn&&Judge_Over(turn)) return 40000000;//如果对手被将死 
	if(dep==Max_dep) return val;
	
	int cnt,st_cid,ed_cid,ans,tmp;
	bool me;
	bool uni_map[20][20];
	Pos st=Pos(-1,-1),ed=Pos(-1,-1);
	if(turn==side) ans=-20000000,me=1;//我方决策, 选取max答案 
	else ans=20000000,me=0;//对手决策, 选取min答案 
	
	priority_queue<Max_Status> Ma_q;Max_Status Ma_p;
	priority_queue<Min_Status> Mi_q;Min_Status Mi_p;
	
	for(int i=0;i<=9;++i)
	for(int j=0;j<=8;++j){
		if(mp[i][j]!=-1&&Belong(mp[i][j])==turn){
			cnt=Print_Move(mp[i][j],Pos(i,j));
			if(cnt!=0){
				for(int r=0;r<=9;++r)
				for(int c=0;c<=8;++c)
				uni_map[r][c]=Move_Map[r][c];
				
				for(int r=0;r<=9;++r)
				for(int c=0;c<=8;++c)
				if(uni_map[r][c]){
					//移动棋子
					st_cid=mp[i][j];mp[i][j]=-1;
					ed_cid=mp[r][c];mp[r][c]=st_cid; 
					if(Checkmate(turn^1,1)){
						mp[i][j]=st_cid;mp[r][c]=ed_cid;
						continue;
					}
					if(dep==0){
						if(side==0&&check_red==5&&Checkmate(0)){
							check_red--;
							mp[i][j]=st_cid;mp[r][c]=ed_cid;
							continue;
						}
						else if(side==1&&check_black==5&&Checkmate(1)){
							check_black--;
							mp[i][j]=st_cid;mp[r][c]=ed_cid;
							continue;
						}
					}
					
//					Print_Board();Sleep(100);
					//评估价值后丢进优先队列 
					if(me) Ma_q.push(Max_Status(Pos(i,j),Pos(r,c),Judge_Val(side)));
					else Mi_q.push(Min_Status(Pos(i,j),Pos(r,c),Judge_Val(side)));
					
					//复原棋子 
					mp[i][j]=st_cid;mp[r][c]=ed_cid;
				}
				
			}
		}
	}
	if(me) while(!Ma_q.empty()){
		Ma_p=Ma_q.top();Ma_q.pop();
		int i=Ma_p.stp.first,j=Ma_p.stp.second,r=Ma_p.edp.first,c=Ma_p.edp.second;
		if(i==-1||j==-1||r==-1||c==-1) puts("-1 ERROR"),system("Pause");
		if(i>9||j>8||r>9||c>8) puts("EDGE ERROR"),system("Pause");
		
		st_cid=mp[i][j];mp[i][j]=-1;
		ed_cid=mp[r][c];mp[r][c]=st_cid;
		tmp=dfs(side,dep+1,turn^1,ans,worst_ans);//深搜博弈树同时交换执棋方 
		//复位棋子
		mp[i][j]=st_cid;mp[r][c]=ed_cid;
		if(tmp>ans){
			ans=tmp;
			st=Pos(i,j);ed=Pos(r,c);
		}
		if(ans>worst_ans) return ans;
		
	}
	else while(!Mi_q.empty()){
		Mi_p=Mi_q.top();Mi_q.pop();
		int i=Mi_p.stp.first,j=Mi_p.stp.second,r=Mi_p.edp.first,c=Mi_p.edp.second;
		if(i==-1||j==-1||r==-1||c==-1) puts("-1 ERROR"),system("Pause");
		if(i>9||j>8||r>9||c>8) puts("EDGE ERROR"),system("Pause");
		
		st_cid=mp[i][j];mp[i][j]=-1;
		ed_cid=mp[r][c];mp[r][c]=st_cid; 
		tmp=dfs(side,dep+1,turn^1,best_ans,ans);//深搜博弈树同时交换执棋方 
		//复位棋子
		mp[i][j]=st_cid;mp[r][c]=ed_cid; 
		if(tmp<ans){
			ans=tmp;
			st=Pos(i,j);ed=Pos(r,c);
		}
		if(ans<best_ans) return ans;
	}
	if(dep==0){
//		puts("HERE!!");
		printf("(%d,%d) to (%d,%d)\n",st.first,st.second,ed.first,ed.second);
		if(st==Pos(-1,-1)) puts("ERROR!!!");
		Mail.add_mail(Move_Info(mp[st.first][st.second],st,ed),side);

		OP_Log[side][OP_Round]=Operate_Log(mp[st.first][st.second],mp[ed.first][ed.second],Pos(st.first,st.second),Pos(ed.first,ed.second));
		mp[ed.first][ed.second]=mp[st.first][st.second];
		mp[st.first][st.second]=-1;
		
	}
//	cout<<ans<<"<--\n";
	return ans;
}
bool One_Punch;//猎杀时刻 
void Game(bool P=0){
	Player=P;One_Punch=0;
	Print_Board();
	check_black=0;check_red=0;
	Pos pos_st,pos_ed;
	int cid,ans=0,p1,p2;
	while(1){
		Sleep(100);
		
		/*  红棋  */
		if(Player==0){
			while(1){
				pos_st=Player_Operate(Player);
				Sleep(40);
				if(pos_st==Pos(99,99)){
					if(OP_Round==0) continue;
					if(One_Punch){
						Mail.add_mail("落子无悔!",0);
						continue;
					}
					OP_Round--;
					Mail.add_mail("悔棋",0);
					//撤回对方棋步 
					Pos sp=OP_Log[1][OP_Round].st_pos,ep=OP_Log[1][OP_Round].ed_pos;
					int sc=OP_Log[1][OP_Round].st_cid,ec=OP_Log[1][OP_Round].ed_cid;
					mp[sp.first][sp.second]=sc;mp[ep.first][ep.second]=ec;
//					cout<<sc<<" & "<<ec<<"!\n";
					//撤回己方棋步 
					sp=OP_Log[0][OP_Round].st_pos;ep=OP_Log[0][OP_Round].ed_pos;
					sc=OP_Log[0][OP_Round].st_cid;ec=OP_Log[0][OP_Round].ed_cid;
					mp[sp.first][sp.second]=sc;mp[ep.first][ep.second]=ec;
					
//					cout<<OP_Round<<"<--\n";
					continue;
				}
				cid=mp[pos_st.first][pos_st.second];
				Print_Move(cid,pos_st);
				pos_ed=Player_Move();
				if(pos_ed==Pos(-1,-1)) continue;
				Sleep(40);
				
				//移动棋子 
				OP_Log[0][OP_Round]=Operate_Log(mp[pos_st.first][pos_st.second],mp[pos_ed.first][pos_ed.second],Pos(pos_st.first,pos_st.second),Pos(pos_ed.first,pos_ed.second));
				
				p2=mp[pos_ed.first][pos_ed.second];mp[pos_ed.first][pos_ed.second]=cid;
				p1=mp[pos_st.first][pos_st.second];mp[pos_st.first][pos_st.second]=-1;
				
				if(Checkmate(1,1)){
					Mail.add_mail("不可送将!",0);
					mp[pos_st.first][pos_st.second]=p1;
					mp[pos_ed.first][pos_ed.second]=p2;
					continue;
				}
				Mail.add_mail(Move_Info(OP_Log[0][OP_Round].st_cid,OP_Log[0][OP_Round].st_pos,OP_Log[0][OP_Round].ed_pos),0);
				break;
			}
			
		}
		else{
			Print_Board();
			Sleep(100);
			ans=dfs(0,0,0);
			if(ans>10000000&&One_Punch==0) Mail.add_mail("猎杀时刻",0),One_Punch=1;
		}
		Print_Board();
		if(Judge_Over(1)){
			Print_Board();
			putimage_transparent(NULL,win_pic,100,150,transColor);
			Sleep(1000);
			Mail.add_mail("红方获胜!",0);
			return ;
		}
		if(Checkmate(0)) check_red++,Sleep(1000);
		else check_red=0;
		
		
		/*  黑棋  */
		if(Player==1){
			while(1){
				pos_st=Player_Operate(Player);
				Sleep(40);
				if(pos_st==Pos(99,99)){
					if(OP_Round==0) continue;
					if(One_Punch){
						Mail.add_mail("落子无悔!",1);
						continue;
					}
					Mail.add_mail("悔棋",0);
					//撤回对方棋步 
					Pos sp=OP_Log[0][OP_Round].st_pos,ep=OP_Log[0][OP_Round].ed_pos;
					int sc=OP_Log[0][OP_Round].st_cid,ec=OP_Log[0][OP_Round].ed_cid;
					mp[sp.first][sp.second]=sc;mp[ep.first][ep.second]=ec;
//					cout<<sc<<" & "<<ec<<"!\n";
					
					//撤回己方棋步 
					OP_Round--;
					sp=OP_Log[1][OP_Round].st_pos;ep=OP_Log[1][OP_Round].ed_pos;
					sc=OP_Log[1][OP_Round].st_cid;ec=OP_Log[1][OP_Round].ed_cid;
					mp[sp.first][sp.second]=sc;mp[ep.first][ep.second]=ec;
					
//					cout<<OP_Round<<"<--\n";
					continue;
				}
				cid=mp[pos_st.first][pos_st.second];
				Print_Move(cid,pos_st);
				pos_ed=Player_Move();
				if(pos_ed==Pos(-1,-1)) continue;
				Sleep(40);
				
				//移动棋子 
				OP_Log[1][OP_Round]=Operate_Log(mp[pos_st.first][pos_st.second],mp[pos_ed.first][pos_ed.second],Pos(pos_st.first,pos_st.second),Pos(pos_ed.first,pos_ed.second));
				
				p2=mp[pos_ed.first][pos_ed.second];mp[pos_ed.first][pos_ed.second]=cid;
				p1=mp[pos_st.first][pos_st.second];mp[pos_st.first][pos_st.second]=-1;
				if(Checkmate(0,1)){
					Mail.add_mail("不可送将!",1);
					mp[pos_st.first][pos_st.second]=p1;
					mp[pos_ed.first][pos_ed.second]=p2;
					continue;
				}
				Mail.add_mail(Move_Info(OP_Log[1][OP_Round].st_cid,OP_Log[1][OP_Round].st_pos,OP_Log[1][OP_Round].ed_pos),1);
				break;
			}
			
		}
		else{
			Print_Board();
			Sleep(100);
			ans=dfs(1,0,1);
			if(ans>10000000&&One_Punch==0) Mail.add_mail("猎杀时刻",1),One_Punch=1;
		}
		Print_Board();
		
		if(Judge_Over(0)){
			Print_Board();
			putimage_transparent(NULL,win_pic,100,150,transColor);
			Sleep(1000);
			Mail.add_mail("黑方获胜!",1);
			return ;
		}
		if(Checkmate(1)) check_black++,Sleep(1000);
		else check_black=0;
		OP_Round++;
		
//		pos_ed=Get_Move();
//		AI();
	}
}

void Menu(){
	Load_Pic();Set_Chess();
	setcolor(EGERGB(0xEC,0x22,0x22));
	putimage(150,0,board_pic);xyprintf(300,20,"选择先后手");
	putimage_transparent(NULL,Red_pic,250,240,transColor);
	putimage_transparent(NULL,Black_pic,470,240,transColor);
	
	int MX,MY,status=3,st;
	bool push=0,side=0,sel=0;
	//250 260
	
	while(1){
		mousepos(&MX,&MY);
		if(KEY_DOWN(VK_LBUTTON)) push=1;
		if(push&&KEY_UP(VK_LBUTTON)) sel=1;
		
		if(MX>=250&&MX<=375&&MY>=240&&MY<=365) st=1;
		else if(MX>=470&&MX<=595&&MY>=240&&MY<=365) st=2;
		else st=3;
		if(st!=status){
			status=st;
			putimage(150,0,board_pic);xyprintf(300,20,"选择先后手");
			if(st==1) putimage_transparent(NULL,Red_pic,250,230,transColor);
			else putimage_transparent(NULL,Red_pic,250,240,transColor);
			
			if(st==2) putimage_transparent(NULL,Black_pic,470,230,transColor);
			else putimage_transparent(NULL,Black_pic,470,240,transColor);
		}
		
		if(sel==1){
			if(st==1){
				for(int X=250;X<=360;X+=5){
					putimage(150,0,board_pic);xyprintf(370,20,"红方");
					putimage_transparent(NULL,Red_pic,X,230,transColor);
					Sleep(15);
				}
				Sleep(1000);
				Game(0);
				return ;
			}
			else if(st==2){
				for(int X=470;X>=360;X-=5){
					putimage(150,0,board_pic);xyprintf(370,20,"黑方");
					putimage_transparent(NULL,Black_pic,X,230,transColor);
					Sleep(15);
				}
				Sleep(1000);
				Game(1);
				return ;
			}
			sel=0;push=0;
			continue;
		}
		Sleep(100);
	}
}

int main(){
	initgraph(840,600);
	setfont(50,0,"华文行楷");setbkmode(TRANSPARENT);
	Menu();
	Print_Board();Sleep(100);
//	setfillcolor(EGERGB(0x77,0xdd,0xff));
//	bar(0,0,500,500);
//	PIMAGE pimg = newimage();
//	getimage(pimg, "车.png");
	system("Pause");
	return 0;
}
/*
8col * 9row
size = 60 * 60
background = (8*60+30*2) * (9*60+30*2) = 540 * 600
*/
