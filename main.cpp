#include "CAFrame.h"

int main(int argc, char* argv[])
{
	Location_t l(5,5);
	CAValue v1((int)S_RIGHT);
	CAValue v2((int)S_UP);
	NetWormLand Land;
	NetWormData W1, W2;
	Player_t p1, p2;
	
	p1 = Land.add_player(pair<int,int>(8,6), &W1);
	W1.Head->set_value(S_RIGHT);
	
	p2 = Land.add_player(pair<int,int>(10,7), &W2);
	W2.Head->set_value(S_UP);
	
	Land.print_me();

	for(int i=0; i<3; ++i)
	{
		Land.calculate_update_headonly();
		
		//Land.calculate_full();
		//Land.update_full();
		
		//if(i==0) W2.Head->set_value(S_RIGHT);
		Land.print_me();

		cout << "WormData body size: " << W2.Body.size() << endl;
		cout  << "WormData head location: (" << W2.Head->get_location().first 
				<< ", " << W2.Head->get_location().second << ")\n" <<  endl;
		if (W2.Head->current_value().ival < 2)
			cout << "Worm died!" << endl;
	}

	return 0;
}
