# 5. Understanding 3D Voxel Rendering & Math

In the previous chapter, we drew a volumetric red line sweeping through the inside of our Cube using `drawLine3D(lineStart, lineEnd, Color::red());`. 

Before we move on to running the code, it is vital to quickly unpack the physical constraints and math principles behind volumetric rendering within the `matrixserver` framework. Managing thousands of data points in a true spatial volume requires grasping a few key concepts.

---

## What is a Voxel?

You are likely familiar with a **Pixel** (Picture Element) — a 2D colored square on a flat screen mapped by $X$ (width) and $Y$ (height).

A **Voxel** (Volumetric Pixel) is a 3D pixel. It represents a single LED point floating in physical space, mapped by $X$ (width), $Y$ (height), and $Z$ (depth). A standard monitor has millions of pixels; however, a physical LED Cube has a much lower resolution due to physical constraints. 

If your LED cube panels are $64 \times 64$, the total volumetric space isn't $64 \times 64$ ($4,096$), but rather $64 \times 64 \times 64$, which equals **$262,144$ voxels**! Even at low spatial resolutions, true 3D arrays represent an enormous amount of data structure that the framework manages for you.

---

## 3D Coordinate Systems

The framework provides an environment constant called `CUBESIZE`. Default to `CUBESIZE = 64`.

Because arrays are 0-indexed, your absolute interior boundaries for any voxel exist from `0` to `CUBESIZE - 1` (or `0` to `63`).

However, the hardware matrix introduces an outer-shell wrap known as the **Virtual Cube**. To truly draw on the outermost physical screen glass, you map coordinates to `VIRTUALCUBEMAXINDEX` (which evaluates to `65`) or `0`.

* **Origin `(1, 1, 1)`:** The true bottom-front-left corner of the internal volumetric space.
* **Surface Shell:** Mapping a coordinate to `x=0` places it strictly on the Left Panel. Mapping `x=65` places it on the Right Panel.

If you attempt to render a voxel at `Vector3i(64, 0, 0)`, it exists completely outside the bounds of the LED Cube and will be safely clipped (ignored) by the rasterizer, preventing crashes.

---

## Volumetric Movement & Boundaries

When animating a voxel (like a bouncing ball or our moving red line), we apply a **Vector** to dictate its trajectory per frame. For example, a voxel at `(32, 32, 32)` with a velocity vector of `(1, 0, 0)` will move to `(33, 32, 32)` on the next frame.

When an object hits the boundary ($> 63$ or $< 0$), you must mathematically choose how it behaves:

1. **Clipping:** Let it exit the volume and disappear (e.g., bullets in a game).
2. **Wrapping:** Use the modulo operator (`%`) to safely wrap it.
3. **Collision / Bouncing:** Invert the trajectory. If velocity is `(1, 0, 0)` and it hits `63`, set velocity to `(-1, 0, 0)` so it travels backwards! (*We used this in our First App:* we multiplied our `direction` by `-1` whenever `zPos` hit the boundaries, causing the red square to bounce smoothly up and down the outer walls).

---

## 3D Vectors in Practice

The framework harnesses the powerful **Eigen** library for its spatial vectors:

* **`Vector3i`**: Used for discrete Voxel coordinates (Integers). LED matrices cannot render "half a lightbulb," so final drawing operations always use `Vector3i`.
* **`Vector3f`**: Used for floating-point calculations like physics engines. Gravity (`-9.81`) or smooth dampening requires precision. Keep your mathematical logic in `Vector3f` and simply cast to `Vector3i` inside the drawing functions!

You can leverage standard linear algebra against these vectors natively:
```cpp
Vector3i posA(10, 10, 10);
Vector3i posB(40, 40, 40);

// Calculate the spatial distance between two points
float distance = (posA - posB).norm();
```
*(This is how you calculate hit-detection between two 3D objects!)*

---

## Drawing Geometric 3D Primitives

When we called `drawLine3D()`, the framework takes your starting origin `Vector3i`, the target destination `Vector3i`, and seamlessly uses algorithms (like an extrapolated *Bresenham's line algorithm*) to interpolate the shortest path across the 3D grid, lighting up every Voxel along the way perfectly.

This geometry abstraction gives you tremendous power:

* Want a solid volumetric box? You can calculate 8 corner nodes and draw 12 lines between them!
* You can define a central cluster of vectors, mathematically apply a Rotational Matrix to spin them on the Y-Axis, and draw them every frame so they rotate elegantly inside the physical enclosure of the cube.

Now that we understand the $262,144$ data constraints of `CUBESIZE` and the mathematical routing used to paint our volumetric shapes, we are officially ready to compile and run our Code!
