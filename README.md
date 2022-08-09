# shaderdoll
Shadertoy clone

# Usage
## Basic template
```
vec3 main()
{
  return vec3(1, 1, 1);
};
```

Produces a white screen.

The program is run for each pixel on the screen, the return value of main will be the color of the pixel on the screen.
`vec3(0, 0, 0)` is black, `vec3(1, 1, 1)` is white. x = red, y = green, z = blue.

Everything has to end with a semicolon - typically `}` will always have a semicolon that follows it unless it's inside of an `if else if else` chain.

## Input variables
```
vec3 main()
{
  return vec3(uv.x, 0, 0);
};
```

Produces a red gradient that gets more intense as x increases. `uv = (0, 0)` represents top left and `uv = (1, 1)` represents bottom right.
`uv` is automatically defined every time the program is run.

## Define a function
```
float function_name(float param, vec3 other_param)
{
  return param;
};
```

Currently, a return statement won't exit the function, it will only set the return value that will be returned at the end of the function.

## Control flow
### If statements
```
if (expr)
{
}
else if (other expr)
{
}
else
{
};
```

## Operators
`+`, `-`, `*`, `/` work on floats and vectors.

`<` and `>` can compare floats.

Operators can only have the same type on both sides, so if you want to scale a vector for example by 2:
```
vec3 a = vec3(1, 2, 3);
a = a * vec3(2, 2, 2);
```

Unary `-` doesn't exist, use `0-` instead.

## Builtin functions
* `print`: Print values. Debugging purposes typically
* `sqrt`: Square root of a float
* `distance`: Distance between two vectors of variable size
* `dot`: Dot product between two vectors of variable size
* `normalize`: Normalize vector of variable size
* `length`: Length of vector of variable size
* `abs`: Absolute value
* `pow`: pow(base, exp)
* `sin`: sine
* `cos`: cosine
* `tan`: tangent
* `asin`: arcsin
* `acos`: arccos
* `atan`: arctan
