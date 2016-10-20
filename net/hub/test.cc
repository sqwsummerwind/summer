
#include <stdio.h>
#include <set>

class Test
{
	public:
		Test():
		topics_(std::set<int>())
		{}
		void display()
		{
			printf("%d\n", static_cast<int>(topics_.size()));
		}
	private:
		std::set<int> topics_;
};


int main(int argc, char* argv[])
{
	Test test;
	test.display();
	return 0;
}

