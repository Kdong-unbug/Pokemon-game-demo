#include <iostream>
#include <string>

using namespace std;


class Skill
{
public:
	string Skillname;//技能名
	int power;//威力
	int type;//属性
	double critRate;//暴击概率
	int effect;//附带属性 中毒睡眠灼烧等
	
	
	Skill(string Skillname,int power,int type,double critRate,int effect)
	{
		this->Skillname = Skillname;
		this->power = power;
		this->type = type;
		this->critRate = critRate;
		this->effect = effect;
		
	}

		

};

class Pokemon
{
public:
	string Pokemonname;//精灵名
	int level;//等级
	int exp;//当前经验值
	int maxexp;//升级所需经验值
	int hp;//当前血量;
	int maxhp;//最大血量;
	int attact;//攻击力;
	int defense;//防御力;
	int elemType;//属性
	
	
};

int main()
{
	return 0;
}
