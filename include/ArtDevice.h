#ifndef ArtDev
#define ArtDev
#include <cstring>

class ArtDevice
{
private:
    int m_id;
    char m_name[40];

public:
    ArtDevice(int id, char name[])
    {
        m_id = id;
        strcpy(m_name, name);
    }
};

#endif