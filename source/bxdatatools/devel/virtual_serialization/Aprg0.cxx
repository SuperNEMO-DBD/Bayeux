//

#include <iostream>
#include <exception>

#include <Ac1.hpp>
#include <Ac2.hpp>

int main (void)
{
  try
    {
      A::c1 o1 (666);
      std::cout << "o1 : " << o1 << std::endl;

      A::c2 o2 ("test");
      std::cout << "o2 : " << o2 << std::endl;

    }
  catch (std::exception &x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}
