#include<iostream>
#include<iomanip>
#include<string>

int time_limit = 0;

enum class camp_type { Blue, Red, None };

int timer = 0;
int counter = 0;
const int Round = 10;
const int minutes[Round] = { 0,5,10,20,30,35,38,40,50,55 };

void init();

void print_time()
{
	std::cout << std::setw(3) << std::setfill('0') << timer << ":" << std::setw(2) << std::setfill('0') << minutes[counter] << " ";
};

bool exceed_time_limits() { return timer * 60 + minutes[counter] > time_limit; };

class Headquarter;

class Warrior;

class Iceman;

class Lion;

class Wolf;

class Weapon;

class City;

class City
{
public:
	City(int i, int init_ele = 0) :blue_warrior(nullptr), red_warrior(nullptr), ahead(nullptr), next(nullptr), id(i), flag(camp_type::None), \
		blue_win(0), red_win(0), elements(init_ele) {};

	~City() {};

	City* next, * ahead;

	camp_type flag;

	Warrior* blue_warrior, * red_warrior;

	camp_type judge_first()
	{
		if (flag == camp_type::Blue || ((flag == camp_type::None) && (id % 2==0))) return camp_type::Blue;
		else return camp_type::Red;
	};

	void CityBattle(City*,City*);

	inline void gain_elements() { elements += 10; };

	void CityShoot();
  
	void check_flag();

	void offer_elements(City*,Warrior*);

	const int id;

private:
	int blue_win, red_win;
protected:
	int elements;
};

class Weapon
{
public:
	enum class type { sword, bomb, arrow };

	Weapon(int init_endure, type t, int init_damage) :endurance(init_endure), thistype(t), damage(init_damage) {};

	static const int weapon_kinds = 3;

	int damage;

	virtual void endurance_loss() = 0;

	inline bool check_endurance() { return (endurance > 0); };

	const type thistype;
protected:
	int endurance;
};

class Sword :public Weapon
{
	friend class Warrior;
public:
	Sword(int init_damage) :Weapon(init_damage, type::sword, init_damage){};
	virtual void endurance_loss() { endurance = endurance * 8 / 10; damage = endurance; }
};

class Bomb :public Weapon
{
public:
	Bomb() :Weapon(basic_endurance, type::bomb, INT32_MAX) {};
	virtual void endurance_loss() { endurance--; return; }
private:
	static const int basic_endurance = 1;
};

class Arrow :public Weapon
{
	friend class Warrior;
	friend void init();
public:
	Arrow() :Weapon(basic_endurance, type::arrow, basic_damage) {};
	virtual void endurance_loss() { endurance--; return; }
private:
	static int basic_damage;
	static const int basic_endurance = 3;
};

int Arrow::basic_damage = 0;

class Warrior
{
	friend class Wolf;
	friend class Sword;	
	friend void City::CityBattle(City*, City*);
public:
	enum class kind { Iceman, Lion, Wolf, Ninja, Dragon };

	static const int warrior_kinds = 5;

	Warrior(int temph, int tempi, int tempa, kind k, camp_type t, City* c) :health(temph), thiscity(c), \
		id(tempi), damage(tempa), thiskind(k), camp(t), sword(nullptr), arrow(nullptr), bomb(nullptr),lastkill(false)
	{
		if(thiskind!=Warrior::kind::Lion&&thiskind!=Warrior::kind::Wolf) init_weapon(id);
	};

	void init_weapon(int id);

	inline virtual bool check_alive() { return (health > 0); }

	virtual void born_attribute() {};

	virtual void skill() {};

	void show_weapon() const;

	inline bool have_arrow() { return (arrow != nullptr); };
	inline bool have_bomb() { return (bomb != nullptr); };

	virtual std::string show_name() const = 0;

	void DeleteWarrior();

	void DeleteWeapon(Weapon::type);

	void use_sword_hurt(Warrior*);

	void use_arrow_hurt(Warrior*);

	void use_bomb_hurt(Warrior*);

	virtual void fight_back(Warrior*);

	virtual void march();

	int try_fight_back(int);

	int try_attack(int);

	bool check_will_be_killed(Warrior*);

	inline int show_health() const { return health; }

	inline int show_force() const { return damage; }

	virtual ~Warrior()
	{
		delete sword;
		delete arrow;
		delete bomb;
	};

	const kind thiskind;

	void get_award(Headquarter*);

protected:
	int health, id, damage;

	Sword* sword;
	Arrow* arrow;
	Bomb* bomb;

	bool lastkill;

	const camp_type camp;

	City* thiscity;
};

class Headquarter :public City
{
	friend void Warrior::get_award(Headquarter*);
public:
	Headquarter(int init_elem) : City(0, init_elem) {};

	~Headquarter() {};

	Warrior* Make_Warrior(camp_type t);

	inline static void init() { id[0] = id[1] = 1; };

	void report_elements(camp_type t) {
		std::cout << elements << " elements in ";
		if (t == camp_type::Blue) std::cout << "blue headquarter" << std::endl;
		else std::cout << "red headquarter" << std::endl;
		return;
	}

private:
	static int id[2];
};

void Warrior::get_award(Headquarter* src)
{
	if (lastkill)
	{
		lastkill = false;
		int temp = src->elements - 8;
		if (temp >= 0)
		{
			health += 8;
			src->elements = temp;
		}
	}
	return;
}

void Warrior::init_weapon(int id)
{
	switch (id % 3)
	{
	case 0:
		sword = new Sword(damage * 2 / 10); 
		if (!sword->check_endurance()) DeleteWeapon(Weapon::type::sword);
		break;
	case 1:
		bomb = new Bomb(); break;
	case 2:
		arrow = new Arrow(); break;
	}
	return;
}

void Warrior::DeleteWeapon(Weapon::type t)
{
	switch (t)
	{
	case Weapon::type::sword:
		delete sword;
		sword = nullptr;
		break;
	case Weapon::type::bomb:
		delete bomb;
		bomb = nullptr;
		break;
	case Weapon::type::arrow:
		delete arrow;
		arrow = nullptr;
		break;
	}
	return;
}

void Warrior::DeleteWarrior() {
	if (this == nullptr) return;
	if (camp == camp_type::Blue)
		thiscity->blue_warrior = nullptr;
	else thiscity->red_warrior = nullptr;
	delete this;
	return;
};

void Warrior::march()
{
	if (camp == camp_type::Blue)
	{
		thiscity->ahead->blue_warrior = thiscity->blue_warrior;
		thiscity->blue_warrior = nullptr;
		thiscity = thiscity->ahead;
	}
	else
	{
		thiscity->next->red_warrior = thiscity->red_warrior;
		thiscity->red_warrior = nullptr;
		thiscity = thiscity->next;
	}
	return;
}

bool Warrior::check_will_be_killed(Warrior* another)
{
	if (camp == thiscity->judge_first())
	{
		if (try_attack(another->health) > 0)
			return another->try_fight_back(health) <= 0;
	}
	else return another->try_attack(health) <= 0;
	return false;
}

void Warrior::use_sword_hurt(Warrior* another)
{
	if (sword)
	{
		another->health -= damage + sword->damage;
		sword->endurance_loss();
		if (!sword->check_endurance()) DeleteWeapon(Weapon::type::sword);
	}
	else another->health -= damage;
	print_time();
	std::cout << show_name() << " attacked " << another->show_name() << " in city " << thiscity->id << " with "
		<< show_health() << " elements and force " << show_force() << std::endl;
	return;
}

void Warrior::use_bomb_hurt(Warrior* another)
{
	print_time();
	std::cout << show_name() << " used a bomb and killed " << another->show_name() << std::endl;
	another->DeleteWarrior(), DeleteWarrior();
}

void Warrior::use_arrow_hurt(Warrior* another)
{
	another->health -= arrow->damage;
	arrow->endurance_loss();
	if (!arrow->check_endurance()) DeleteWeapon(Weapon::type::arrow);
	return;
}

void Warrior::fight_back(Warrior* another)
{
	if (sword)
	{
		another->health -= damage / 2 + sword->damage;
		sword->endurance_loss();
		if (!sword->check_endurance()) DeleteWeapon(Weapon::type::sword);
	}
	else another->health -= damage / 2;
	print_time();
	std::cout << show_name() << " fought back against " << another->show_name() << " in city " << thiscity->id << std::endl;
	return;
};

int Warrior::try_fight_back(int temp_health)
{
	if (thiskind == Warrior::kind::Ninja) return temp_health;
	if (sword)
		return temp_health - damage / 2 - sword->damage;
	else return temp_health - damage / 2;
}

int Warrior::try_attack(int temp_health)
{
	if (sword)
		return temp_health - damage - sword->damage;
	else return temp_health - damage;
}

void Warrior::show_weapon() const
{
	using std::cout;
	bool flag = false;
	cout << show_name() << " has ";
	if (arrow)
	{
		cout << "arrow(" << arrow->endurance << ")";
		flag = true;
	}
	if (bomb)
	{
		if (flag) cout << ",";
		else flag = true;
		cout << "bomb";
	}
	if (sword)
	{
		if (flag) cout << ',';
		else flag = true;
		cout << "sword(" << sword->endurance << ")";
	}
	if (!flag)cout << "no weapon";
	cout << std::endl;
};

class Iceman :public Warrior
{
	friend Warrior* Headquarter::Make_Warrior(camp_type);
	friend void init();
public:
	Iceman(camp_type t, int total_amount, City* c) :Warrior(basic_health, total_amount, basic_damage, Warrior::kind::Iceman, t, c),damage_or_not(false) {
	};
	~Iceman() {};

	virtual std::string show_name() const
	{
		std::string temp;
		if (camp == camp_type::Blue) temp = "blue ";
		else temp = "red ";
		temp += "iceman ";
		temp.append(std::to_string(id));
		return temp;
	}

	virtual void march()
	{
		if (damage_or_not)
		{
			damage_or_not = false;
			if (health > 9) health -= 9;
			else health = 1;
			damage += 20;
		}
		else damage_or_not = true;
		Warrior::march();
	}

private:
	static int basic_health;

	static int basic_damage;

	bool damage_or_not;
};

class Lion :public Warrior
{
	friend Warrior* Headquarter::Make_Warrior(camp_type);
	friend void City::CityBattle(City*, City*);
	friend void init();
public:
	Lion(camp_type t, int total_amount, int init_loyalty, City* c) :Warrior(basic_health, total_amount, basic_damage, Warrior::kind::Lion, t, c), \
		loyalty(init_loyalty), last_health(basic_health) {};

	void march()
	{
		Warrior::march();
	}

	void skill()
	{
		if (minutes[counter] == 40)
			loyalty -= loyalty_loss;
		else if (loyalty <= 0)
		{
			print_time();
			std::cout << show_name() << " ran away" << std::endl;
			DeleteWarrior();
		}
		return;
	}

	void born_attribute() { std::cout << "Its loyalty is " << loyalty << std::endl; return; }

	virtual std::string show_name() const
	{
		std::string temp;
		if (camp == camp_type::Blue) temp = "blue ";
		else temp = "red ";
		temp += "lion ";
		temp.append(std::to_string(id));
		return temp;
	}

	inline bool check_alive()
	{
		if (health <= 0) return false;
		last_health = health;
		return true;
	}

private:
	static int basic_health;

	static int basic_damage;

	static int loyalty_loss;

	int loyalty;

	int last_health;
};

class Wolf :public Warrior
{
	friend Warrior* Headquarter::Make_Warrior(camp_type);
	friend void init();
public:
	Wolf(camp_type t, int total_amount, City* c) :Warrior(basic_health, total_amount, basic_damage, Warrior::kind::Wolf, t, c) {};

	~Wolf() {};

	virtual std::string show_name() const
	{
		std::string temp;
		if (camp == camp_type::Blue) temp = "blue ";
		else temp = "red ";
		temp += "wolf ";
		temp.append(std::to_string(id));
		return temp;
	}

	virtual void skill()
	{
		if (camp == camp_type::Blue)
			get_weapon(thiscity->red_warrior);
		else
			get_weapon(thiscity->blue_warrior);
	}

	void get_weapon(Warrior*);

private:
	static int basic_health;

	static int basic_damage;

};

void Wolf::get_weapon(Warrior* another)
{
	if ((sword == nullptr) && (another->sword != nullptr))
	{
		sword = another->sword;
		another->sword = nullptr;
	}
	if ((arrow == nullptr) && (another->arrow != nullptr))
	{
		arrow = another->arrow;
		another->arrow = nullptr;
	}
	if ((bomb == nullptr) && (another->bomb != nullptr))
	{
		bomb = another->bomb;
		another->bomb = nullptr;
	}
	return;
}

class Ninja :public Warrior
{
	friend Warrior* Headquarter::Make_Warrior(camp_type);
	friend void init();
public:
	Ninja(camp_type t, int total_amount, City* c) :Warrior(basic_health, total_amount, basic_damage, Warrior::kind::Ninja, t, c)
	{
		init_weapon(id + 1);
	};

	virtual std::string show_name() const
	{
		std::string temp;
		if (camp == camp_type::Blue) temp = "blue ";
		else temp = "red ";
		temp += "ninja ";
		temp.append(std::to_string(id));
		return temp;
	}

	virtual void fight_back(Warrior*) {};
private:
	static int basic_health;

	static int basic_damage;
};

class Dragon :public Warrior
{
	friend Warrior* Headquarter::Make_Warrior(camp_type);
	friend void init();
public:
	Dragon(camp_type t, int total_amount, int init_morale, City* c) :Warrior(basic_health, total_amount, basic_damage, Warrior::kind::Dragon, t, c), \
		morale((double)init_morale / basic_health) {};

	void born_attribute() { std::cout << "Its morale is " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << morale << std::endl; }

	virtual std::string show_name() const
	{
		std::string temp;
		if (camp == camp_type::Blue) temp = "blue ";
		else temp = "red ";
		temp += "dragon ";
		temp.append(std::to_string(id));
		return temp;
	}

	virtual void skill() {
		bool flag1 = false;
		if (camp == camp_type::Blue)
			flag1 = thiscity->red_warrior->check_alive();
		else flag1 = thiscity->blue_warrior->check_alive();
		if (flag1) morale -= 0.2;
		else morale += 0.2;
		if (morale > 0.8 &&  camp==thiscity->judge_first())
		{
			print_time();
			std::cout << show_name() << " yelled in city " << thiscity->id << std::endl;
		}
		return;
	};

private:
	static int basic_health;

	static int basic_damage;

	double morale;
};

int Iceman::basic_health = 0;
int Iceman::basic_damage = 0;

int Lion::basic_damage = 0;
int Lion::basic_health = 0;
int Lion::loyalty_loss = 0;

int Wolf::basic_damage = 0;
int Wolf::basic_health = 0;

int Dragon::basic_damage = 0;
int Dragon::basic_health = 0;

int Ninja::basic_damage = 0;
int Ninja::basic_health = 0;

int Headquarter::id[2] = { 1,1 };

const Warrior::kind sequence[2][5] = {
	{Warrior::kind::Wolf, Warrior::kind::Lion,Warrior::kind::Dragon,Warrior::kind::Ninja,Warrior::kind::Iceman},
	{Warrior::kind::Dragon, Warrior::kind::Iceman,Warrior::kind::Lion,Warrior::kind::Wolf,Warrior::kind::Ninja }
};

void City::CityShoot()
{
	using std::cout;
	using std::endl;
	if (red_warrior != nullptr && red_warrior->have_arrow())
	{
		if (next->blue_warrior != nullptr)
		{
			red_warrior->use_arrow_hurt(next->blue_warrior);
			print_time();
			cout << red_warrior->show_name() << " shot";
			if (!next->blue_warrior->check_alive())
				cout << " and killed " << next->blue_warrior->show_name();
			cout << endl;
		}
	}
	if (blue_warrior != nullptr && blue_warrior->have_arrow())
	{
		if (ahead->red_warrior != nullptr)
		{
			blue_warrior->use_arrow_hurt(ahead->red_warrior);
			print_time();
			cout << blue_warrior->show_name() << " shot";
			if (!ahead->red_warrior->check_alive())
				cout << " and killed " << ahead->red_warrior->show_name();
			cout << endl;
		}
	}
	return;
}

void City::check_flag()
{
	if (blue_win >= 2)
	{
		if (flag == camp_type::None||flag==camp_type::Red)
		{
			flag = camp_type::Blue;
			print_time();
			std::cout << "blue flag raised in city " << id << std::endl;
		}
	}
	else if (red_win >= 2)
	{
		if (flag == camp_type::None||flag==camp_type::Blue)
		{
			flag = camp_type::Red;
			print_time();
			std::cout << "red flag raised in city " << id << std::endl;
		}
	}
	return;
};

void City::CityBattle(City* red,City* blue)
{
	using std::cout;
	using std::endl;
	bool return_or_not = false;
	if (blue_warrior == nullptr || red_warrior == nullptr)
	{
		if (blue_warrior && !blue_warrior->check_alive()) blue_warrior->DeleteWarrior();
		if (red_warrior && !red_warrior->check_alive()) red_warrior->DeleteWarrior();
		return;
	}
	bool flag1 = blue_warrior->check_alive(), flag2 = red_warrior->check_alive();
	if (!flag1 && !flag2)
		blue_warrior->DeleteWarrior(), red_warrior->DeleteWarrior();
	else if (flag1 && flag2)
	{
		Warrior* attacker, * defenser;
		if (judge_first() == camp_type::Blue)
			attacker = blue_warrior, defenser = red_warrior;
		else
			attacker = red_warrior, defenser = blue_warrior;
		attacker->use_sword_hurt(defenser);
		if (defenser->check_alive()) defenser->fight_back(attacker);
		else
		{
			print_time();
			cout << defenser->show_name() << " was killed in city " << id << endl;
			if (defenser->thiskind == Warrior::kind::Lion)  attacker->health += dynamic_cast<Lion*>(defenser)->last_health;
			attacker->lastkill = true;
		}
		if (!attacker->check_alive())
		{
			print_time();
			cout << attacker->show_name() << " was killed in city " << id << endl;
			if (attacker->thiskind == Warrior::kind::Lion)  defenser->health += dynamic_cast<Lion*>(attacker)->last_health;
			defenser->lastkill = true;
		};
		bool blue_alive = blue_warrior->check_alive(), red_alive = red_warrior->check_alive();
		if (blue_alive && red_alive)
		{
			blue_win = red_win = 0;
			if (red_warrior->thiskind == Warrior::kind::Dragon||red_warrior->thiskind==Warrior::kind::Lion) red_warrior->skill();
			if (blue_warrior->thiskind == Warrior::kind::Dragon||blue_warrior->thiskind==Warrior::kind::Lion) blue_warrior->skill();
		}
		else if (blue_alive)
		{
			blue_win++, red_win = 0;
			if (blue_warrior->thiskind == Warrior::kind::Wolf || blue_warrior->thiskind == Warrior::kind::Dragon)
				blue_warrior->skill();
			offer_elements(blue, blue_warrior);
			red_warrior->DeleteWarrior();
		}
		else if (red_alive)
		{
			red_win++, blue_win = 0;
			if (red_warrior->thiskind == Warrior::kind::Wolf || red_warrior->thiskind == Warrior::kind::Dragon)
				red_warrior->skill();
			offer_elements(red, red_warrior);
			blue_warrior->DeleteWarrior();
		}
	}
	else if (flag1)
	{
		blue_win++, red_win = 0;
		if (blue_warrior->thiskind == Warrior::kind::Wolf || blue_warrior->thiskind == Warrior::kind::Dragon)
			blue_warrior->skill();
		offer_elements(blue, blue_warrior);
		red_warrior->DeleteWarrior();
		blue_warrior->lastkill = true;
	}
	else
	{
		red_win++, blue_win = 0;
		if (red_warrior->thiskind == Warrior::kind::Wolf || red_warrior->thiskind == Warrior::kind::Dragon)
			red_warrior->skill();
		offer_elements(red, red_warrior);
		blue_warrior->DeleteWarrior();
		red_warrior->lastkill = true;
	}
	check_flag();
	return;
}

void City::offer_elements(City* dst,Warrior* w)
{
	print_time();
	std::cout << w->show_name() << " earned " << elements << " elements for his headquarter" << std::endl;
	dst->elements += elements;
	elements = 0;
	return;
};

Warrior* Headquarter::Make_Warrior(camp_type t)
{
	using namespace std;
	int temp = static_cast<int>(t);
	int Nextid = id[temp];
	Warrior* p = nullptr;
	switch (sequence[temp][Nextid % Warrior::warrior_kinds])
	{
	case Warrior::kind::Iceman:
	{
		int tempp = elements - Iceman::basic_health;
		if (tempp >= 0)
		{
			p = new Iceman(t, Nextid, this);
			elements = tempp;
		}
		break;
	}
	case Warrior::kind::Lion:
	{
		int tempp = elements - Lion::basic_health;
		if (tempp >= 0)
		{
			p = new Lion(t, Nextid, tempp, this);
			elements = tempp;
		}
		break;
	}
	case Warrior::kind::Wolf:
	{
		int tempp = elements - Wolf::basic_health;
		if (tempp >= 0)
		{
			p = new Wolf(t, Nextid, this);
			elements = tempp;
		}
		break;
	}
	case Warrior::kind::Ninja:
	{
		int tempp = elements - Ninja::basic_health;
		if (tempp >= 0)
		{
			p = new Ninja(t, Nextid, this);
			elements = tempp;
		}
		break;
	}
	case Warrior::kind::Dragon:
	{
		int tempp = elements - Dragon::basic_health;
		if (tempp >= 0)
		{
			p = new Dragon(t, Nextid, tempp, this);
			elements = tempp;
		}
		break;
	}
	}
	if (p)
	{
		print_time();
		cout << p->show_name() << " born" << endl;
		id[temp]++;
		p->born_attribute();
	}
	return p;
}

void CreateCity(City*& first, City*& last, int city_amount)
{
	if (city_amount == 1)
	{
		first = last = new City(1);
		first->next = first->ahead = last->next = last->ahead = nullptr;
	}
	else
	{
		first = new City(1);
		City* temp = first, * follow = first;
		first->ahead = nullptr;
		for (int i = 2; i <= city_amount; i++)
		{
			temp->next = new City(i);
			temp = temp->next;
			temp->ahead = follow;
			follow = temp;
		}
		last = temp;
		last->next = nullptr;
	}
	return;
};

void init()
{
	timer = counter = 0;
	std::cin >> Arrow::basic_damage >> Lion::loyalty_loss >> time_limit
		>> Dragon::basic_health >> Ninja::basic_health >> Iceman::basic_health >> Lion::basic_health >> Wolf::basic_health
		>> Dragon::basic_damage >> Ninja::basic_damage >> Iceman::basic_damage >> Lion::basic_damage >> Wolf::basic_damage;
	Headquarter::init();
	return;
}

void print_march(Warrior* warrior, int cityid)
{
	print_time();
	std::cout << warrior->show_name() << " marched to city " << cityid << " with "
		<< warrior->show_health() << " elements and force " << warrior->show_force() << std::endl;
	return;
};

void Clear(City*& first)
{
	City* temp = first;
	while (temp->next != nullptr)
	{
		temp->blue_warrior->DeleteWarrior();
		temp->red_warrior->DeleteWarrior();
		temp = temp->next;
		delete temp->ahead;
	}
	return;
};

int main()
{
	using std::cin;
	using std::cout;
	using std::endl;
	int test_amount = 0;
	cin >> test_amount;
	for (int i = 1; i <= test_amount; i++)
	{
		int temp_ele = 0, city_amount = 0;
		cin >> temp_ele >> city_amount;
		Headquarter blue(temp_ele), red(temp_ele);
		City* FirstCity = nullptr, * LastCity = nullptr;
		CreateCity(FirstCity, LastCity, city_amount);
		blue.ahead = LastCity, red.next = FirstCity, LastCity->next = &blue, FirstCity->ahead = &red;
		Warrior* useless_red_warrior = nullptr, * useless_blue_warrior = nullptr;
		init();
		cout << "Case " << i << ":" << endl;
		bool checkwin = false;
		while (true)
		{
			//第一回合，双方武士降生
			red.red_warrior = red.Make_Warrior(camp_type::Red);
			blue.blue_warrior = blue.Make_Warrior(camp_type::Blue);
			counter++;
			if (exceed_time_limits()) break;

			//第二回合，检测Lion逃跑情况
			for (City* Thecity = FirstCity->ahead; Thecity != nullptr; Thecity = Thecity->next)
			{
				if (Thecity->red_warrior != nullptr)
					if (Thecity->red_warrior->thiskind == Warrior::kind::Lion)
						Thecity->red_warrior->skill();
				if (Thecity->blue_warrior != nullptr)
					if (Thecity->blue_warrior->thiskind == Warrior::kind::Lion)
						Thecity->blue_warrior->skill();
			}
			counter++;
			if (exceed_time_limits()) break;

			//第三回合，前进
			for (City* Thecity = LastCity; Thecity != nullptr; Thecity = Thecity->ahead)
				if (Thecity->red_warrior != nullptr)
					Thecity->red_warrior->march();
			for (City* Thecity = FirstCity; Thecity != nullptr; Thecity = Thecity->next)
				if (Thecity->blue_warrior != nullptr)
					Thecity->blue_warrior->march();
			if (red.blue_warrior != nullptr)
			{
				print_time();
				Warrior*& rbw = red.blue_warrior;
				cout << rbw->show_name() << " reached red headquarter with " << rbw->show_health() << " elements and force " << rbw->show_force() << endl;
				if (useless_blue_warrior==nullptr)
				{
					useless_blue_warrior = rbw;
					red.blue_warrior = nullptr;
				}
				else
				{
					print_time();
					cout << "red headquarter was taken" << endl;
					checkwin = true;
				}
			}
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
			{
				if (Thecity->red_warrior != nullptr) print_march(Thecity->red_warrior, Thecity->id);
				if (Thecity->blue_warrior != nullptr) print_march(Thecity->blue_warrior, Thecity->id);
			}
			if (blue.red_warrior != nullptr)
			{
				Warrior*& brw = blue.red_warrior;
				print_time();
				cout << brw->show_name() << " reached blue headquarter with " << brw->show_health() << " elements and force " << brw->show_force() << endl;
				if (useless_red_warrior == nullptr)
				{
					useless_red_warrior = brw;
					blue.red_warrior = nullptr;
				}
				else
				{
					print_time();
					cout << "blue headquarter was taken" << endl;
					checkwin = true;
				}
			}
			if (checkwin) break;
			counter++;
			if (exceed_time_limits()) break;

			//第四回合，产出生命元
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
				Thecity->gain_elements();
			counter++;
			if (exceed_time_limits()) break;

			//第五回合，取走生命元
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
				if (Thecity->blue_warrior == nullptr && Thecity->red_warrior == nullptr) continue;
				else if (Thecity->blue_warrior != nullptr && Thecity->red_warrior != nullptr) continue;
				else if (Thecity->blue_warrior) Thecity->offer_elements(&blue,Thecity->blue_warrior);
				else 	Thecity->offer_elements(&red,Thecity->red_warrior);
			counter++;
			if (exceed_time_limits()) break;

			//第六回合，放箭
			for (City* Thecity = FirstCity->ahead; Thecity != nullptr; Thecity = Thecity->next)
			{
				if (Thecity->blue_warrior == nullptr && Thecity->red_warrior == nullptr) continue;
				Thecity->CityShoot();
			}
			counter++;
			if (exceed_time_limits()) break;

			//第七回合，使用炸弹
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
			{
				if (Thecity->blue_warrior == nullptr || Thecity->red_warrior == nullptr) continue;
				Warrior* b = Thecity->blue_warrior, * r = Thecity->red_warrior;
				if (b->check_alive() && r->check_alive())
				{
					if (b->have_bomb() && b->check_will_be_killed(r)) b->use_bomb_hurt(r);
					else if (r->have_bomb() && r->check_will_be_killed(b)) r->use_bomb_hurt(b);
				}
			}
			counter++;
			if (exceed_time_limits()) break;

			//第八回合，发生战斗，武士获取奖励
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
				Thecity->CityBattle(&red,&blue);
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
				if (Thecity->red_warrior == nullptr) continue;
				else Thecity->red_warrior->get_award(&red);
			for (City* Thecity = LastCity; Thecity !=FirstCity->ahead ; Thecity=Thecity->ahead)
				if (Thecity->blue_warrior == nullptr) continue;
				else Thecity->blue_warrior->get_award(&blue);

			counter++;
			if (exceed_time_limits()) break;

			//第九回合，报告生命元数量
			print_time();
			red.report_elements(camp_type::Red);
			print_time();
			blue.report_elements(camp_type::Blue);
			counter++;
			if (exceed_time_limits()) break;

			//第十回合，武士报告武器情况
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
			{
				if (Thecity->red_warrior != nullptr)
				{
					print_time();
					Thecity->red_warrior->show_weapon();
				}
			}
			if (useless_red_warrior)
			{
				print_time();
				useless_red_warrior->show_weapon();
			}	
			if (useless_blue_warrior)
			{
				print_time();
				useless_blue_warrior->show_weapon();
			}
			for (City* Thecity = FirstCity; Thecity != LastCity->next; Thecity = Thecity->next)
			{
				if (Thecity->blue_warrior != nullptr)
				{
					print_time();
					Thecity->blue_warrior->show_weapon();
				}
			}
			counter = 0;
			timer++;
			if (exceed_time_limits()) break;
		}
		Clear(FirstCity);
		red.blue_warrior->DeleteWarrior(), red.red_warrior->DeleteWarrior();
		blue.blue_warrior->DeleteWarrior(), blue.red_warrior->DeleteWarrior();
	}
	return 0;
}
