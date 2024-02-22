#include "includes/Utils.hpp"
#include "includes/Users.hpp"

int	Utils::binarySearch(std::vector<std::string> users, std::string nickname, unsigned int size)
{
	int low = 0;
	int high = size - 1;
	int mid;

	while (high >= low)
	{
		mid = (high + low) / 2;
		if (users[mid] == nickname)
			return mid;
		else if (users[mid] < nickname)
			low = mid + 1;
		else if (users[mid] > nickname)
			high = mid - 1;
	}
	return -1;
}

int	Utils::user_exists(std::string nickname)
{
	if (binarySearch(Users::nickname_, nickname, Users::userList_.size()) != -1)
		return 0;
	return -1;
}