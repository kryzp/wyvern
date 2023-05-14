# wyvern

![](https://github.com/kryzp/wyvern/blob/master/images/logo.png)

3D Engine / Playground for learning stuff. Features it's own "home-brewed" container types like vector, string, queue, etc... mostly because I hate relying on std:: since it makes me uncomfortable for some reason, I just prefer to have full control over how really essential datastructures like that work.

Naming Convention when logging: ```"[NAMESPACE(s):CLASS(|DEBUG/RELEASE IF ONLY APPLIES TO ONE)] message"```  

I use a fully left-handed coordinate system because I'm left-handed. A notable effect of this is that the cross product will make use of the left hand rule instead of the right hand rule.  

"Why don't you use shared pointers as much as you should do?" - I don't know. They just make me feel uneasy for some reason, so I usually just try to control the lifetime manually through ``new`` and ``delete``.

Warning: Super unstable, and constantly changing - your projects *will* break!  
This is your final warning!

Credit to https://vulkan-tutorial.com for helping me learn vulkan!
