#ifndef CAFRAME_H
#define CAFRAME_H

// States that a cell may be in
#define S_EMPTY 0
#define S_TAIL 1
#define S_UP 2
#define S_DOWN 3
#define S_LEFT 4
#define S_RIGHT 5

// Types of calculates for the NetWormLand::Calculate() function.
#define CALC_FULL 0
#define CALC_HEADONLY 1

#include <iostream>
#include <utility> //for pair<>
#include <map>
#include <deque>
using namespace std;

typedef pair<int, int> Location_t;
typedef int Player_t;

class NetWormData;
class NetWormLand;

class CAValue
{
	public:
	CAValue() : bval(false) {}
	CAValue(int i) : ival(i) {}
	CAValue(bool b) : bval(b) {}
	CAValue(float f) : fval(f) {}
	~CAValue() {}
	
	union 
	{
		bool bval;
		int ival;
		float fval;
	};
};

// The interface for a CA cell
class CACell
{
	public:
	virtual ~CACell() {}
	
	virtual CAValue current_value() = 0;
	virtual CAValue last_value() = 0;
	virtual CAValue next_value() = 0;
	virtual void exec_rule() = 0;
	virtual void exec_update() = 0;
	virtual void set_value(CAValue) = 0;
	virtual void set_neighbor(Location_t, CACell*) = 0;
	virtual void set_bodydata(NetWormData*) = 0;
	virtual NetWormData* get_bodydata() = 0;
	virtual void clear_bodydata() = 0;
	virtual Location_t get_location() = 0;

	protected:
};

class NetWormData
{
	public:
	NetWormData() {}
	~NetWormData() {}

	CACell* Head;
	deque<CACell*> Body;
};

CACell* create_wormhead(Location_t, NetWormLand*, int);

class NetWormLand
{
	public:
	NetWormLand() : player_num(0) {}
	~NetWormLand() {}

	CACell* GetCell(int x, int y)
	{
		return GetCell(pair<int, int>(x,y));
	}
	
	CACell* GetCell(Location_t l)
	{
		map<Location_t, CACell*>::iterator itr = TheLand.find(l);
		if (itr != TheLand.end())
			return itr->second;
		else
			return 0;
	}

	bool PutCell(Location_t l, CACell* pc)
	{
		CACell* pcg = GetCell(l);
		if(!pcg)
		{
			TheLand[l] = pc;
			return true;
		}
		else
			return false;
	}

	Player_t add_player(Location_t l, NetWormData* wd)
	{
		Player_t my_num;
		CAValue v((int)S_RIGHT);
		CACell* cp = this->GetCell(l);
		if(cp)
			cp->set_value(v);
		else
			cp = create_wormhead(l, this, S_RIGHT);

		cp->set_bodydata(wd); //FIXME possible memory leak here

		// add this player to the roster
		my_num = player_num;
		Worms[my_num] = wd;
		++player_num;
		
		return my_num;
	}

	void calculate_full()
	{
		map<Location_t, CACell*>::iterator itr = TheLand.begin();
		while (itr != TheLand.end())
		{
			itr->second->exec_rule();
			++itr;
		}
	}

	void calculate_update_headonly()
	{
		map<Player_t, NetWormData*>::iterator itr = Worms.begin();
		map<Location_t, CACell*> active_cells;
		map<Location_t, CACell*>::iterator ac_itr;
		Location_t l;
		while(itr != Worms.end())
		{
			if(!player_is_dead(itr->second))
			{
				l = itr->second->Head->get_location();
				Location_t l2 = l;
				active_cells[l] = TheLand[l];

				l2.first = l.first+1;
				l2.second = l.second;
				active_cells[l2] = TheLand[l2];
				
				l2.first = l.first;
				l2.second = l.second+1;
				active_cells[l2] = TheLand[l2];
				
				l2.first = l.first-1;
				l2.second = l.second;
				active_cells[l2] = TheLand[l2];
				
				l2.first = l.first;
				l2.second = l.second-1;
				active_cells[l2] = TheLand[l2];
				
			}
			++itr;
		}

		ac_itr = active_cells.begin();
		while(ac_itr != active_cells.end())
		{
			//cout 	<< "exec_rule @: (" << ac_itr->first.first 
			//		<< ',' << ac_itr->first.second << ")" << endl;
			ac_itr->second->exec_rule();
			++ac_itr;
		}

		ac_itr = active_cells.begin();
		while(ac_itr != active_cells.end())
		{
			//cout 	<< "exec_update @: (" << ac_itr->first.first 
			//		<< ',' << ac_itr->first.second << ")" << endl;
			ac_itr->second->exec_update();
			++ac_itr;
		}
	}

	void update_full()
	{
		map<Location_t, CACell*>::iterator itr = TheLand.begin();
		while (itr != TheLand.end())
		{
			itr->second->exec_update();
			++itr;
		}
	}

	void update_headonly()
	{
		map<Player_t, NetWormData*>::iterator itr = Worms.begin();
		map<Location_t, CACell*> active_cells;
		map<Location_t, CACell*>::iterator ac_itr;
		Location_t l;
		while(itr != Worms.end())
		{
			if(1) //!player_is_dead(itr->second))
			{
				l = itr->second->Head->get_location();
				Location_t l2 = l;
				active_cells[l] = TheLand[l];
				
				l2.first = l.first+1;
				l2.second = l.second;
				active_cells[l2] = TheLand[l2];
				
				l2.first = l.first;
				l2.second = l.second+1;
				active_cells[l2] = TheLand[l2];
				
				l2.first = l.first-1;
				l2.second = l.second;
				active_cells[l2] = TheLand[l2];
				
				l2.first = l.first;
				l2.second = l.second-1;
				active_cells[l2] = TheLand[l2];
			}
			++itr;
		}

		ac_itr = active_cells.begin();
		while(ac_itr != active_cells.end())
		{
			cout 	<< "exec_update @: (" << ac_itr->first.first 
					<< ',' << ac_itr->first.second << ")" << endl;
			ac_itr->second->exec_update();
			++ac_itr;
		}
	}

	void print_me()
	{
		cout << '\t';
		for(int i=0; i<20; ++i)
			cout << i%10;
		cout << endl;
		for(int row=0; row<20; ++row)
		{
			cout << row << '\t';
			for(int col=0; col<20; ++col)
			{
				CACell* c = GetCell(row,col);
				if (!c)
					cout << ".";
				else if(c->current_value().ival == S_RIGHT)
					cout << ">";
				else if(c->current_value().ival == S_LEFT)
					cout << "<";
				else if(c->current_value().ival == S_UP)
					cout << "^";
				else if(c->current_value().ival == S_DOWN)
					cout << "V";
				else if(c->current_value().ival == S_TAIL)
					cout << "#";
				else if(c->current_value().ival == S_EMPTY)
					cout << "_";
			}
			cout << endl;
		}
		cout << endl;
	}

	bool player_is_dead(NetWormData* pd)
	{
		if(!pd->Head)
			return true;

		if(pd->Head->current_value().ival < 2)
			return true;
		else
			return false;
	}

	void remove_worm(NetWormData* pd)
	{
		if(!pd->Head) return;
		CACell* cp;
		
		deque<CACell*>::iterator itr = pd->Body.begin();
		while(itr != pd->Body.end())
		{
			cp = *itr;
			cp->set_value(S_EMPTY);
			cp->clear_bodydata();
			++itr;
		}

		pd->Head = NULL;
		pd->Body.clear();
	}

	private:
	map<Location_t, CACell*> TheLand;
	map<Player_t, NetWormData*> Worms;
	Player_t player_num;
};


//implements a 'null' cell.
//There can be only one instance of this type.  But
//there will likely be many references to that instance.
//This is used for any cell which we know must be dead.
class NULL_NetWormCell : public CACell
{
	public:
	static NULL_NetWormCell* Instance()
	{
		if (!_instance)
			_instance = new NULL_NetWormCell();
		return _instance;
	}
	
	~NULL_NetWormCell() {}
	
	CAValue current_value() {return CGvalue;}
	CAValue last_value() {return CGvalue;}
	CAValue next_value() {return CGvalue;}
	void set_value(CAValue v) {} 
	void set_neighbor(Location_t l, CACell* c) {}
	void set_bodydata(NetWormData* p) {}
	NetWormData* get_bodydata() { return NULL; }
	void clear_bodydata() {}
	Location_t get_location() {Location_t l(-1, -1); return l;}

	void exec_rule() {}
	void exec_update() {}

	private:
	NULL_NetWormCell() : CGvalue(S_EMPTY) {}
	static NULL_NetWormCell* _instance;
	CAValue CGvalue;
};
NULL_NetWormCell* NULL_NetWormCell::_instance = 0;

//implements a Life cell.
//This is used whenever a cell's value becomes known.
class NetWormCell : public CACell
{
	public:
	NetWormCell(Location_t l, NetWormLand* pl, int value, bool add_neighbors=true)
		: PGvalue(S_EMPTY), CGvalue(S_EMPTY), NGvalue(S_EMPTY),
		  OurLand(pl), Location(l), BodyData(NULL)
	{
		if(!pl->PutCell(l, this))
		{
			cerr	<< "Error: A cell already exists at (" 
					<< l.first << ", " << l.second
					<< "). Possible memory leak!" << endl;  
		}

		//get pointers to my neighbors
		Right = pl->GetCell(l.first, l.second + 1);
		Left = pl->GetCell(l.first, l.second - 1);
		Up = pl->GetCell(l.first - 1, l.second);
		Down = pl->GetCell(l.first + 1, l.second);

				/*if(neighbors[r+1][c+1])
					cout << "Got ";
				else
					cout << "Null "; 
				cout << l.first + r << ", " << l.second + c << endl;
				*/

		validate_neighbors(add_neighbors);

		CGvalue = value;

	}
	~NetWormCell() {}
	
	CAValue current_value() {return CGvalue;}
	CAValue last_value() {return PGvalue;}
	CAValue next_value() {return NGvalue;}
	void set_value(CAValue v) { CGvalue = v; add_skin();}
	void set_neighbor(Location_t l, CACell* c)
	{
		/*cout	<< "Passed in: " << l.first << ", " << l.second << '\n'
				<< "Me:        " << Location.first << ", " << Location.second
				<< endl;
		*/	
		if ((l.first == Location.first) && (l.second == Location.second + 1))
			Right = c;
		else if ((l.first == Location.first) && (l.second == Location.second - 1))
			Left = c;
		else if ((l.first == Location.first - 1) && (l.second == Location.second))
			Up = c;
		else if ((l.first == Location.first + 1) && (l.second == Location.second))
			Down = c;
	}


	void exec_rule() 
	{
		//cout<< Location.first 
		//		<< ", " << Location.second << ": " << endl;

		if (CGvalue.ival == S_UP ||
				CGvalue.ival == S_DOWN ||
				CGvalue.ival == S_LEFT ||
				CGvalue.ival == S_RIGHT )
		{
			NGvalue.ival = S_TAIL;
		}
		else if(CGvalue.ival == S_EMPTY)
		{
			int count = 0;
			if((Right->current_value()).ival == S_LEFT)
			{
				NGvalue.ival = S_LEFT;
				set_bodydata(Right->get_bodydata());
				++count;
			}
			if((Left->current_value()).ival == S_RIGHT)
			{
				NGvalue.ival = S_RIGHT;
				set_bodydata(Left->get_bodydata());
				++count;
			}
			if((Down->current_value()).ival == S_UP)
			{
				NGvalue.ival = S_UP;
				set_bodydata(Down->get_bodydata());
				++count;
			}
			if((Up->current_value()).ival == S_DOWN)
			{
				NGvalue.ival = S_DOWN;
				set_bodydata(Up->get_bodydata());
				++count;
			}

			// check to see if two or more heads are colliding on me
			if (count > 1)
				NGvalue.ival = S_TAIL;
		}
	}

	void exec_update() 
	{
		PGvalue = CGvalue;
		CGvalue = NGvalue;

		if (PGvalue.ival == S_EMPTY && CGvalue.ival != S_EMPTY)
			add_skin();
	}

	void set_bodydata(NetWormData* d)
	{ 
		BodyData = d;
		BodyData->Head = this;
		BodyData->Body.push_front(this);
	}

	NetWormData* get_bodydata() { return BodyData; }
	void clear_bodydata() {BodyData = NULL;}

	Location_t get_location() {return Location;}
	
	private:
	//functions

	//for each neighbor: it it dosent exist, set the pointer to the 'NULL' cell
	//otherwise, give myself to it
	void validate_neighbors(bool add_neighbors)
	{
		if(add_neighbors)
			cout << "run validate on " 
				<< Location.first << ", " << Location.second << endl;

		//Do we have a valid cell pointer here?
		if (!Right)
		{
			// nope.  Should we add this guy?
			if(add_neighbors)
			{
				// Create a new cell here.
				int row = Location.first;
				int col = Location.second + 1;
				Right = new NetWormCell(pair<int,int>(row,col), OurLand, S_EMPTY, false);
				cout << "add in validate: " << row << ", " << col << endl;
			}
			else
				// Just use a null cell instance.
				Right = NULL_NetWormCell::Instance();
		}
		else
		{
			// yes. tell him to use me.
			Right->set_neighbor(Location, this);
		}

		if (!Left)
		{
			if(add_neighbors)
			{
				int row = Location.first;
				int col = Location.second - 1;
				Left = new NetWormCell(pair<int,int>(row,col), OurLand, S_EMPTY, false);
				cout << "add in validate: " << row << ", " << col << endl;
			}
			else
				Left = NULL_NetWormCell::Instance();
		}
		else
		{
			Left->set_neighbor(Location, this);
		}

		if (!Up)
		{
			if(add_neighbors)
			{
				int row = Location.first - 1;
				int col = Location.second;
				Up = new NetWormCell(pair<int,int>(row,col), OurLand, S_EMPTY, false);
				cout << "add in validate: " << row << ", " << col << endl;
			}
			else
				Up = NULL_NetWormCell::Instance();
		}
		else
		{
			Up->set_neighbor(Location, this);
		}

		if (!Down)
		{
			if(add_neighbors)
			{
				int row = Location.first + 1;
				int col = Location.second;
				Down = new NetWormCell(pair<int,int>(row,col), OurLand, S_EMPTY, false);
				cout << "add in validate: " << row << ", " << col << endl;
			}
			else
				Down = NULL_NetWormCell::Instance();
		}
		else
		{
			Down->set_neighbor(Location, this);
		}
	}

	void add_skin()
	{
		//cout << "add_skin on " 
		//	<< Location.first << ", " << Location.second << endl;
		
		if (!Right || Right == NULL_NetWormCell::Instance())
		{
			int row = Location.first;
			int col = Location.second + 1;
			Right = new NetWormCell(pair<int,int>(row,col), OurLand, 0, false);
			//			cout << "add skin: " << row << ", " << col << endl;
		}

		if (!Left || Left == NULL_NetWormCell::Instance())
		{
			int row = Location.first;
			int col = Location.second - 1;
			Left = new NetWormCell(pair<int,int>(row,col), OurLand, 0, false);
			//			cout << "add skin: " << row << ", " << col << endl;
		}

		if (!Up || Up == NULL_NetWormCell::Instance())
		{
			int row = Location.first - 1;
			int col = Location.second;
			Up = new NetWormCell(pair<int,int>(row,col), OurLand, 0, false);
			//			cout << "add skin: " << row << ", " << col << endl;
		}

		if (!Down || Down == NULL_NetWormCell::Instance())
		{
			int row = Location.first + 1;
			int col = Location.second;
			Down = new NetWormCell(pair<int,int>(row,col), OurLand, 0, false);
			//			cout << "add skin: " << row << ", " << col << endl;
		}

	}

	//Variables
	NetWormLand* OurLand;
	NetWormData* BodyData;
	Location_t Location;
	CAValue PGvalue, CGvalue, NGvalue;
	CACell* Right;
	CACell* Left;
	CACell* Up;
	CACell* Down;
};

CACell* add_networm_cell(Location_t l, NetWormLand* lp, CAValue v)
{
	CACell* cp = lp->GetCell(l);
	if(cp)
		cp->set_value(v);
	else
		cp = new NetWormCell(l, lp, v.ival);

	return cp;
}

CACell* create_wormhead(Location_t l, NetWormLand* n, int v)
{
	return new NetWormCell(l, n, v);
}


#endif
