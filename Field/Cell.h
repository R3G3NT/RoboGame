#ifndef CELL_H_
#define CELL_H_

class Robot;

enum class Terrain { Floor, Wall, Swamp };

class Cell {
private:
    Terrain terrain = Terrain::Floor;
    bool visible = false; 
    Robot* robot = nullptr;

public:
    Cell(Terrain p_terrain);

    Terrain GetTerrain() const;
    bool IsWalkable() const;
    bool IsVisible() const;

    void SetTerrain(Terrain terrain);
    void SetVisible(bool visible);

    Robot* GetRobot() const;
    bool HasRobot() const;
    void SetRobot(Robot* other);
    void RemoveRobot();
};

#endif
