#import sys
#sys.path.append('/usr/local/lib/tamgu')
import pytamgu

u= """
Overrated and not good.
I was highly disappointed in the food at Pagoda of East-Vietnam -.
The lemon chicken with almonds from the US that tasted like sticky sweet donuts and the honey walnut prawns, the few they actually give you... were not good.
The prices are outrageous, especially since the food was actually less satisfying than most neighborhood Chinese establishments.
Nice ambience, but highly overrated place.
I will not go back.
"""

pytamgu.load("rules.tmg","",1)
v=analyse(u)

print v
