//
// Created by dyanakiev on 3/5/2024.
//

#include "Person.h"
#include <string>

public class Person
{
    private:
    {
        int p_id;
        string p_name;
        int p_age;
        string p_placeOfBirth;
    }

    public:
    {
        public int getId()
        {
            return p_id;
        }

        public void setId(int id)
        {
            p_id = id;
        }
    }
};