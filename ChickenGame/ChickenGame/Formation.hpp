#pragma once
#include <vector>
#include <SFML\System\Vector2.hpp>


class Formation {
public:
	enum Type {
	FourFourTwo,
	ThreeFourThree,
	FormationCount
};
	Formation();
	~Formation();
	Formation(Formation::Type formationType);

	sf::Vector2f getTeamMemberPosition(bool isATeam, int teamMember) const;
	void initialiseTeams(Formation::Type formationType);
private:
	std::vector<sf::Vector2f> ATeamPositions, BTeamPositions;
};