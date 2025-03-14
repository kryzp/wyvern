# wyvern

## 2025 UPDATE:
PLEASE NOTE THAT THIS IS PRETTY BAD CODE. I MADE THIS FOR MY NEA, AND WHILE SOME PARTS ARE STILL PRETTY GOOD-ISH, OTHERS (NOTABLY THE RENDERING ENGINE) ARE PRETTY BADLY WRITTEN, AND INDICATIVE OF POOR UNDERSTANDING OF VULKANS ARCHITECTURE. IF YOU WANT TO SEE BETTER RENDERING CODE, YOU CAN SEE MY OTHER PROJECT ![LILYTHORN](https://github.com/kryzp/lilythorn).

![](https://github.com/kryzp/wyvern/blob/master/images/logo.png)

3D Engine / Playground for learning stuff. Features its own "home-brewed" container types like vector, string, queue, etc... mostly because I hate relying on std:: since it makes me uncomfortable for some reason, I just prefer to have full control over how really essential datastructures like that work.

Naming Convention when logging: ```"[NAMESPACE(s):CLASS(|DEBUG/RELEASE IF ONLY APPLIES TO ONE)] message"```  

I use a fully left-handed coordinate system because I'm left-handed. A notable effect of this is that the cross product will make use of the left hand rule instead of the right hand rule.  

Warning: Super unstable, and constantly changing - your projects *will* break!  
***This is your final warning!***

### Credits
 * Alexander Overvoorde (https://vulkan-tutorial.com) for helping me learn vulkan!  
 * Sean Barrett for ```stb_*.h``` single-header files!
