Initial Guide

INTRODUCTION

This plugin focus on delivering the basic foundation to create portals in VR that achieve a seameslly teleport. At the same time, the feature has a flat-screen version. The inspiration for this project are games like Tea for God Traversal or Spellbound.

- Links and videos

I believe that this kind of experiences and the locomotion that involves (no artificial movement, just walk) should be further explore to bring better experiences to XR. This project provide an initial framework to develop this feature and solutions for the initial difficult challenges ( Hmd timing, visual fidelity of the portal image...).

DISCLAIMER: All the code is open and free to be modified and improved. At the same time, this was developed during a 2 month BUAS (Breda university of applied science) project. You also should expect some bugs, problems and limited features because of the limited development time.

MAIN ASSETS EXPLANATION

The plugin provides from high level to low:

Portal

HOW TO USE



CURRENT LIMITATIONS

What the current state provides:

**Visualas**

- Portal visual provides a high fidelity effect, both in VR and flat screen. It does not consider ilumination / shadows through the portal.

- Through the material provided in the plugin content the visuals can be expanded / modified.

- Portal has no recursion functionality ( watching another portal image through a portal).

**Teleporting**

- Automatic teleporting when overlap with portal BoxCollider makes a fluid teleport of all dynamic objects. It works very well for VR walking motion currently.

- Automatic teleport can be deactivated and create your own implementation using PortalTools helping functions.

- Teleporting does not consider momentum and other features like rays. It can be further develop with PortalTools helping functions.

- Teleporting crashes if trying to modify the scale of the portals

**Object replication**

- Not Object replication, it can be develop using PortalTools helping functions.

**Performance and systems considerations**

- Performance impact, specially on XR is expected. There is not performance improvements for the rendering of portals. It is something to consider and develop.

- There is not performance strategies for big number of portals, neither system to manage IRL (In real life) floor and portals position for continuos walking. o