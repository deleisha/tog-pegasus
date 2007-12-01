#include "Resource.h"

CIMPLE_NAMESPACE_BEGIN

Resource resource;

Resource::Resource()
{
    Auto_Mutex am(mutex);

    Employee* e;
    e = Employee::create(true);
    e->Id.set(4001);
    e->First.set("Homer");
    e->Last.set("Simpson");
    e->Gender.set(1);
    e->Active.set(true);
    e->OutOfOffice.set(true);
    employees.append(e);

    e = Employee::create(true);
    e->Id.set(4002);
    e->First.set("Carl");
    e->Last.set("Carlson");
    e->Gender.set(1);
    e->Active.set(true);
    employees.append(e);

    e = Employee::create(true);
    e->Id.set(4003);
    e->First.set("Lenny");
    e->Last.set("Leonard");
    e->Gender.set(1);
    e->Active.set(true);
    employees.append(e);
}

Resource::~Resource()
{
    Auto_Mutex am(mutex);

    for (size_t i = 0; i < employees.size(); i++)
        Employee::destroy(employees[i]);
}

CIMPLE_NAMESPACE_END
