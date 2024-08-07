---
slug: /en/sql-reference/functions/type-conversion-functions
sidebar_position: 185
sidebar_label: Type Conversion
---

# Type Conversion Functions

## Common Issues with Data Conversion

ClickHouse generally uses the [same behavior as C++ programs](https://en.cppreference.com/w/cpp/language/implicit_conversion).

`to<type>` functions and [cast](#cast) behave differently in some cases, for example in case of [LowCardinality](../data-types/lowcardinality.md): [cast](#cast) removes [LowCardinality](../data-types/lowcardinality.md) trait `to<type>` functions don't. The same with [Nullable](../data-types/nullable.md), this behaviour is not compatible with SQL standard, and it can be changed using [cast_keep_nullable](../../operations/settings/settings.md/#cast_keep_nullable) setting.

:::note
Be aware of potential data loss if values of a datatype are converted to a smaller datatype (for example from `Int64` to `Int32`) or between
incompatible datatypes (for example from `String` to `Int`). Make sure to check carefully if the result is as expected.
:::

Example:

```sql
SELECT
    toTypeName(toLowCardinality('') AS val) AS source_type,
    toTypeName(toString(val)) AS to_type_result_type,
    toTypeName(CAST(val, 'String')) AS cast_result_type

┌─source_type────────────┬─to_type_result_type────┬─cast_result_type─┐
│ LowCardinality(String) │ LowCardinality(String) │ String           │
└────────────────────────┴────────────────────────┴──────────────────┘

SELECT
    toTypeName(toNullable('') AS val) AS source_type,
    toTypeName(toString(val)) AS to_type_result_type,
    toTypeName(CAST(val, 'String')) AS cast_result_type

┌─source_type──────┬─to_type_result_type─┬─cast_result_type─┐
│ Nullable(String) │ Nullable(String)    │ String           │
└──────────────────┴─────────────────────┴──────────────────┘

SELECT
    toTypeName(toNullable('') AS val) AS source_type,
    toTypeName(toString(val)) AS to_type_result_type,
    toTypeName(CAST(val, 'String')) AS cast_result_type
SETTINGS cast_keep_nullable = 1

┌─source_type──────┬─to_type_result_type─┬─cast_result_type─┐
│ Nullable(String) │ Nullable(String)    │ Nullable(String) │
└──────────────────┴─────────────────────┴──────────────────┘
```

## toInt8

Converts an input value to a value of type [`Int8`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toInt8(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt8('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int8](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.  
For example: `SELECT toInt8(128) == -128;`.
:::

**Returned value**

- 8-bit integer value. [Int8](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toInt8(-8),
    toInt8(-8.8),
    toInt8('-8')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt8(-8):   -8
toInt8(-8.8): -8
toInt8('-8'): -8
```

**See also**

- [`toInt8OrZero`](#toint8orzero).
- [`toInt8OrNull`](#toint8ornull).
- [`toInt8OrDefault`](#toint8ordefault).

## toInt8OrZero

Like [`toInt8`](#toint8), this function converts an input value to a value of type [Int8](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toInt8OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of ordinary Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt8OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 8-bit integer value if successful, otherwise `0`. [Int8](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers. 
:::

**Example**

Query:

``` sql
SELECT
    toInt8OrZero('-8'),
    toInt8OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt8OrZero('-8'):  -8
toInt8OrZero('abc'): 0
```

**See also**

- [`toInt8`](#toint8).
- [`toInt8OrNull`](#toint8ornull).
- [`toInt8OrDefault`](#toint8ordefault).

## toInt8OrNull

Like [`toInt8`](#toint8), this function converts an input value to a value of type [Int8](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toInt8OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt8OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 8-bit integer value if successful, otherwise `NULL`. [Int8](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt8OrNull('-8'),
    toInt8OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt8OrNull('-8'):  -8
toInt8OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toInt8`](#toint8).
- [`toInt8OrZero`](#toint8orzero).
- [`toInt8OrDefault`](#toint8ordefault).

## toInt8OrDefault

Like [`toInt8`](#toint8), this function converts an input value to a value of type [Int8](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toInt8OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `Int8` is unsuccessful. [Int8](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt8OrDefault('0xc0fe', CAST('-1', 'Int8'));`.

:::note
If the input value cannot be represented within the bounds of [Int8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 8-bit integer value if successful, otherwise returns the default value if passed or `0` if not. [Int8](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toInt8OrDefault('-8', CAST('-1', 'Int8')),
    toInt8OrDefault('abc', CAST('-1', 'Int8'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt8OrDefault('-8', CAST('-1', 'Int8')):  -8
toInt8OrDefault('abc', CAST('-1', 'Int8')): -1
```

**See also**

- [`toInt8`](#toint8).
- [`toInt8OrZero`](#toint8orzero).
- [`toInt8OrNull`](#toint8orNull).

## toInt16

Converts an input value to a value of type [`Int16`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toInt16(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt16('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int16](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.  
For example: `SELECT toInt16(32768) == -32768;`.
:::

**Returned value**

- 16-bit integer value. [Int16](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toInt16(-16),
    toInt16(-16.16),
    toInt16('-16')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt16(-16):    -16
toInt16(-16.16): -16
toInt16('-16'):  -16
```

**See also**

- [`toInt16OrZero`](#toint16orzero).
- [`toInt16OrNull`](#toint16ornull).
- [`toInt16OrDefault`](#toint16ordefault).

## toInt16OrZero

Like [`toInt16`](#toint16), this function converts an input value to a value of type [Int16](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toInt16OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt16OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int16](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered as an error.
:::

**Returned value**

- 16-bit integer value if successful, otherwise `0`. [Int16](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt16OrZero('-16'),
    toInt16OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt16OrZero('-16'): -16
toInt16OrZero('abc'): 0
```

**See also**

- [`toInt16`](#toint16).
- [`toInt16OrNull`](#toint16ornull).
- [`toInt16OrDefault`](#toint16ordefault).

## toInt16OrNull

Like [`toInt16`](#toint16), this function converts an input value to a value of type [Int16](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toInt16OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt16OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int16](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 16-bit integer value if successful, otherwise `NULL`. [Int16](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt16OrNull('-16'),
    toInt16OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt16OrNull('-16'): -16
toInt16OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toInt16`](#toint16).
- [`toInt16OrZero`](#toint16orzero).
- [`toInt16OrDefault`](#toint16ordefault).

## toInt16OrDefault

Like [`toInt16`](#toint16), this function converts an input value to a value of type [Int16](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toInt16OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `Int16` is unsuccessful. [Int16](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt16OrDefault('0xc0fe', CAST('-1', 'Int16'));`.

:::note
If the input value cannot be represented within the bounds of [Int16](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 16-bit integer value if successful, otherwise returns the default value if passed or `0` if not. [Int16](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toInt16OrDefault('-16', CAST('-1', 'Int16')),
    toInt16OrDefault('abc', CAST('-1', 'Int16'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt16OrDefault('-16', CAST('-1', 'Int16')): -16
toInt16OrDefault('abc', CAST('-1', 'Int16')): -1
```

**See also**

- [`toInt16`](#toint16).
- [`toInt16OrZero`](#toint16orzero).
- [`toInt16OrNull`](#toint16ornull).

## toInt32

Converts an input value to a value of type [`Int32`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toInt32(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt32('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int32](../data-types/int-uint.md), the result over or under flows. 
This is not considered an error.  
For example: `SELECT toInt32(2147483648) == -2147483648;`
:::

**Returned value**

- 32-bit integer value. [Int32](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toInt32(-32),
    toInt32(-32.32),
    toInt32('-32')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt32(-32):    -32
toInt32(-32.32): -32
toInt32('-32'):  -32
```

**See also**

- [`toInt32OrZero`](#toint32orzero).
- [`toInt32OrNull`](#toint32ornull).
- [`toInt32OrDefault`](#toint32ordefault).

## toInt32OrZero

Like [`toInt32`](#toint32), this function converts an input value to a value of type [Int32](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toInt32OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt32OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int32](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 32-bit integer value if successful, otherwise `0`. [Int32](../data-types/int-uint.md)

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncate fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt32OrZero('-32'),
    toInt32OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt32OrZero('-32'): -32
toInt32OrZero('abc'): 0
```
**See also**

- [`toInt32`](#toint32).
- [`toInt32OrNull`](#toint32ornull).
- [`toInt32OrDefault`](#toint32ordefault).

## toInt32OrNull

Like [`toInt32`](#toint32), this function converts an input value to a value of type [Int32](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toInt32OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt32OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int32](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 32-bit integer value if successful, otherwise `NULL`. [Int32](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt32OrNull('-32'),
    toInt32OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt32OrNull('-32'): -32
toInt32OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toInt32`](#toint32).
- [`toInt32OrZero`](#toint32orzero).
- [`toInt32OrDefault`](#toint32ordefault).

## toInt32OrDefault

Like [`toInt32`](#toint32), this function converts an input value to a value of type [Int32](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toInt32OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `Int32` is unsuccessful. [Int32](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt32OrDefault('0xc0fe', CAST('-1', 'Int32'));`.

:::note
If the input value cannot be represented within the bounds of [Int32](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 32-bit integer value if successful, otherwise returns the default value if passed or `0` if not. [Int32](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toInt32OrDefault('-32', CAST('-1', 'Int32')),
    toInt32OrDefault('abc', CAST('-1', 'Int32'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt32OrDefault('-32', CAST('-1', 'Int32')): -32
toInt32OrDefault('abc', CAST('-1', 'Int32')): -1
```

**See also**

- [`toInt32`](#toint32).
- [`toInt32OrZero`](#toint32orzero).
- [`toInt32OrNull`](#toint32ornull).

## toInt64

Converts an input value to a value of type [`Int64`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toInt64(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported types:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt64('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int64](../data-types/int-uint.md), the result over or under flows. 
This is not considered an error.  
For example: `SELECT toInt64(9223372036854775808) == -9223372036854775808;`
:::

**Returned value**

- 64-bit integer value. [Int64](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toInt64(-64),
    toInt64(-64.64),
    toInt64('-64')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt64(-64):    -64
toInt64(-64.64): -64
toInt64('-64'):  -64
```

**See also**

- [`toInt64OrZero`](#toint64orzero).
- [`toInt64OrNull`](#toint64ornull).
- [`toInt64OrDefault`](#toint64ordefault).

## toInt64OrZero

Like [`toInt64`](#toint64), this function converts an input value to a value of type [Int64](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toInt64OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt64OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int64](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 64-bit integer value if successful, otherwise `0`. [Int64](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt64OrZero('-64'),
    toInt64OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt64OrZero('-64'): -64
toInt64OrZero('abc'): 0
```

**See also**

- [`toInt64`](#toint64).
- [`toInt64OrNull`](#toint64ornull).
- [`toInt64OrDefault`](#toint64ordefault).

## toInt64OrNull

Like [`toInt64`](#toint64), this function converts an input value to a value of type [Int64](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toInt64OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt64OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int64](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 64-bit integer value if successful, otherwise `NULL`. [Int64](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt64OrNull('-64'),
    toInt64OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt64OrNull('-64'): -64
toInt64OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toInt64`](#toint64).
- [`toInt64OrZero`](#toint64orzero).
- [`toInt64OrDefault`](#toint64ordefault).

## toInt64OrDefault

Like [`toInt64`](#toint64), this function converts an input value to a value of type [Int64](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toInt64OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `Int64` is unsuccessful. [Int64](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt64OrDefault('0xc0fe', CAST('-1', 'Int64'));`.

:::note
If the input value cannot be represented within the bounds of [Int64](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 64-bit integer value if successful, otherwise returns the default value if passed or `0` if not. [Int64](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toInt64OrDefault('-64', CAST('-1', 'Int64')),
    toInt64OrDefault('abc', CAST('-1', 'Int64'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt64OrDefault('-64', CAST('-1', 'Int64')): -64
toInt64OrDefault('abc', CAST('-1', 'Int64')): -1
```

**See also**

- [`toInt64`](#toint64).
- [`toInt64OrZero`](#toint64orzero).
- [`toInt64OrNull`](#toint64ornull).

## toInt128

Converts an input value to a value of type [`Int128`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toInt128(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt128('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int128](../data-types/int-uint.md), the result over or under flows. 
This is not considered an error.
:::

**Returned value**

- 128-bit integer value. [Int128](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toInt128(-128),
    toInt128(-128.8),
    toInt128('-128')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt128(-128):   -128
toInt128(-128.8): -128
toInt128('-128'): -128
```

**See also**

- [`toInt128OrZero`](#toint128orzero).
- [`toInt128OrNull`](#toint128ornull).
- [`toInt128OrDefault`](#toint128ordefault).

## toInt128OrZero

Like [`toInt128`](#toint128), this function converts an input value to a value of type [Int128](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toInt128OrZero(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt128OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int128](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 128-bit integer value if successful, otherwise `0`. [Int128](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt128OrZero('-128'),
    toInt128OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt128OrZero('-128'): -128
toInt128OrZero('abc'):  0
```

**See also**

- [`toInt128`](#toint128).
- [`toInt128OrNull`](#toint128ornull).
- [`toInt128OrDefault`](#toint128ordefault).

## toInt128OrNull

Like [`toInt128`](#toint128), this function converts an input value to a value of type [Int128](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toInt128OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt128OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int128](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 128-bit integer value if successful, otherwise `NULL`. [Int128](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt128OrNull('-128'),
    toInt128OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt128OrNull('-128'): -128
toInt128OrNull('abc'):  ᴺᵁᴸᴸ
```

**See also**

- [`toInt128`](#toint128).
- [`toInt128OrZero`](#toint128orzero).
- [`toInt128OrDefault`](#toint128ordefault).

## toInt128OrDefault

Like [`toInt128`](#toint128), this function converts an input value to a value of type [Int128](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toInt128OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `Int128` is unsuccessful. [Int128](../data-types/int-uint.md).

Supported arguments:
- (U)Int8/16/32/64/128/256.
- Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt128OrDefault('0xc0fe', CAST('-1', 'Int128'));`.

:::note
If the input value cannot be represented within the bounds of [Int128](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 128-bit integer value if successful, otherwise returns the default value if passed or `0` if not. [Int128](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toInt128OrDefault('-128', CAST('-1', 'Int128')),
    toInt128OrDefault('abc', CAST('-1', 'Int128'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt128OrDefault('-128', CAST('-1', 'Int128')): -128
toInt128OrDefault('abc', CAST('-1', 'Int128')):  -1
```

**See also**

- [`toInt128`](#toint128).
- [`toInt128OrZero`](#toint128orzero).
- [`toInt128OrNull`](#toint128ornull).

## toInt256

Converts an input value to a value of type [`Int256`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toInt256(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt256('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int256](../data-types/int-uint.md), the result over or under flows. 
This is not considered an error.
:::

**Returned value**

- 256-bit integer value. [Int256](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toInt256(-256),
    toInt256(-256.256),
    toInt256('-256')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt256(-256):     -256
toInt256(-256.256): -256
toInt256('-256'):   -256
```

**See also**

- [`toInt256OrZero`](#toint256orzero).
- [`toInt256OrNull`](#toint256ornull).
- [`toInt256OrDefault`](#toint256ordefault).

## toInt256OrZero

Like [`toInt256`](#toint256), this function converts an input value to a value of type [Int256](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toInt256OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt256OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int256](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 256-bit integer value if successful, otherwise `0`. [Int256](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt256OrZero('-256'),
    toInt256OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt256OrZero('-256'): -256
toInt256OrZero('abc'):  0
```

**See also**

- [`toInt256`](#toint256).
- [`toInt256OrNull`](#toint256ornull).
- [`toInt256OrDefault`](#toint256ordefault).

## toInt256OrNull

Like [`toInt256`](#toint256), this function converts an input value to a value of type [Int256](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toInt256OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toInt256OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [Int256](../data-types/int-uint.md), overflow or underflow of the result occurs. 
This is not considered an error.
:::

**Returned value**

- 256-bit integer value if successful, otherwise `NULL`. [Int256](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toInt256OrNull('-256'),
    toInt256OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt256OrNull('-256'): -256
toInt256OrNull('abc'):  ᴺᵁᴸᴸ
```

**See also**

- [`toInt256`](#toint256).
- [`toInt256OrZero`](#toint256orzero).
- [`toInt256OrDefault`](#toint256ordefault).

## toInt256OrDefault

Like [`toInt256`](#toint256), this function converts an input value to a value of type [Int256](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toInt256OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `Int256` is unsuccessful. [Int256](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`
- String representations of binary and hexadecimal values, e.g. `SELECT toInt256OrDefault('0xc0fe', CAST('-1', 'Int256'));`

:::note
If the input value cannot be represented within the bounds of [Int256](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 256-bit integer value if successful, otherwise returns the default value if passed or `0` if not. [Int256](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toInt256OrDefault('-256', CAST('-1', 'Int256')),
    toInt256OrDefault('abc', CAST('-1', 'Int256'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toInt256OrDefault('-256', CAST('-1', 'Int256')): -256
toInt256OrDefault('abc', CAST('-1', 'Int256')):  -1
```

**See also**

- [`toInt256`](#toint256).
- [`toInt256OrZero`](#toint256orzero).
- [`toInt256OrNull`](#toint256ornull).

# toUInt8

Converts an input value to a value of type [`UInt8`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toUInt8(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt8('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.  
For example: `SELECT toUInt8(256) == 0;`.
:::

**Returned value**

- 8-bit unsigned integer value. [UInt8](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toUInt8(8),
    toUInt8(8.8),
    toUInt8('8')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt8(8):   8
toUInt8(8.8): 8
toUInt8('8'): 8
```

**See also**

- [`toUInt8OrZero`](#touint8orzero).
- [`toUInt8OrNull`](#touint8ornull).
- [`toUInt8OrDefault`](#touint8ordefault).

## toUInt8OrZero

Like [`toUInt8`](#touint8), this function converts an input value to a value of type [UInt8](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toUInt8OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of ordinary Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt8OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 8-bit unsigned integer value if successful, otherwise `0`. [UInt8](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt8OrZero('-8'),
    toUInt8OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt8OrZero('-8'):  0
toUInt8OrZero('abc'): 0
```

**See also**

- [`toUInt8`](#touint8).
- [`toUInt8OrNull`](#touint8ornull).
- [`toUInt8OrDefault`](#touint8ordefault).

## toUInt8OrNull

Like [`toUInt8`](#touint8), this function converts an input value to a value of type [UInt8](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toUInt8OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt8OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 8-bit unsigned integer value if successful, otherwise `NULL`. [UInt8](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt8OrNull('8'),
    toUInt8OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt8OrNull('8'):   8
toUInt8OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toUInt8`](#touint8).
- [`toUInt8OrZero`](#touint8orzero).
- [`toUInt8OrDefault`](#touint8ordefault).

## toUInt8OrDefault

Like [`toUInt8`](#touint8), this function converts an input value to a value of type [UInt8](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toUInt8OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `UInt8` is unsuccessful. [UInt8](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt8OrDefault('0xc0fe', CAST('0', 'UInt8'));`.

:::note
If the input value cannot be represented within the bounds of [UInt8](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 8-bit unsigned integer value if successful, otherwise returns the default value if passed or `0` if not. [UInt8](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toUInt8OrDefault('8', CAST('0', 'UInt8')),
    toUInt8OrDefault('abc', CAST('0', 'UInt8'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt8OrDefault('8', CAST('0', 'UInt8')):   8
toUInt8OrDefault('abc', CAST('0', 'UInt8')): 0
```

**See also**

- [`toUInt8`](#touint8).
- [`toUInt8OrZero`](#touint8orzero).
- [`toUInt8OrNull`](#touint8orNull).

## toUInt16

Converts an input value to a value of type [`UInt16`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toUInt16(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt16('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt16](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.  
For example: `SELECT toUInt16(65536) == 0;`.
:::

**Returned value**

- 16-bit unsigned integer value. [UInt16](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toUInt16(16),
    toUInt16(16.16),
    toUInt16('16')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt16(16):    16
toUInt16(16.16): 16
toUInt16('16'):  16
```

**See also**

- [`toUInt16OrZero`](#touint16orzero).
- [`toUInt16OrNull`](#touint16ornull).
- [`toUInt16OrDefault`](#touint16ordefault).

## toUInt16OrZero

Like [`toUInt16`](#touint16), this function converts an input value to a value of type [UInt16](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toUInt16OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt16OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt16](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered as an error.
:::

**Returned value**

- 16-bit unsigned integer value if successful, otherwise `0`. [UInt16](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt16OrZero('16'),
    toUInt16OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt16OrZero('16'):  16
toUInt16OrZero('abc'): 0
```

**See also**

- [`toUInt16`](#touint16).
- [`toUInt16OrNull`](#touint16ornull).
- [`toUInt16OrDefault`](#touint16ordefault).

## toUInt16OrNull

Like [`toUInt16`](#touint16), this function converts an input value to a value of type [UInt16](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toUInt16OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt16OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt16](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 16-bit unsigned integer value if successful, otherwise `NULL`. [UInt16](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt16OrNull('16'),
    toUInt16OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt16OrNull('16'):  16
toUInt16OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toUInt16`](#touint16).
- [`toUInt16OrZero`](#touint16orzero).
- [`toUInt16OrDefault`](#touint16ordefault).

## toUInt16OrDefault

Like [`toUInt16`](#touint16), this function converts an input value to a value of type [UInt16](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toUInt16OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `UInt16` is unsuccessful. [UInt16](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt16OrDefault('0xc0fe', CAST('0', 'UInt16'));`.

:::note
If the input value cannot be represented within the bounds of [UInt16](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 16-bit unsigned integer value if successful, otherwise returns the default value if passed or `0` if not. [UInt16](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toUInt16OrDefault('16', CAST('0', 'UInt16')),
    toUInt16OrDefault('abc', CAST('0', 'UInt16'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt16OrDefault('16', CAST('0', 'UInt16')):  16
toUInt16OrDefault('abc', CAST('0', 'UInt16')): 0
```

**See also**

- [`toUInt16`](#touint16).
- [`toUInt16OrZero`](#touint16orzero).
- [`toUInt16OrNull`](#touint16ornull).

## toUInt32

Converts an input value to a value of type [`UInt32`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toUInt32(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt32('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt32](../data-types/int-uint.md), the result over or under flows.
This is not considered an error.  
For example: `SELECT toUInt32(4294967296) == 0;`
:::

**Returned value**

- 32-bit unsigned integer value. [UInt32](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toUInt32(32),
    toUInt32(32.32),
    toUInt32('32')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt32(32):    32
toUInt32(32.32): 32
toUInt32('32'):  32
```

**See also**

- [`toUInt32OrZero`](#touint32orzero).
- [`toUInt32OrNull`](#touint32ornull).
- [`toUInt32OrDefault`](#touint32ordefault).

## toUInt32OrZero

Like [`toUInt32`](#touint32), this function converts an input value to a value of type [UInt32](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toUInt32OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt32OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt32](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 32-bit unsigned integer value if successful, otherwise `0`. [UInt32](../data-types/int-uint.md)

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero)
, meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt32OrZero('32'),
    toUInt32OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt32OrZero('32'):  32
toUInt32OrZero('abc'): 0
```
**See also**

- [`toUInt32`](#touint32).
- [`toUInt32OrNull`](#touint32ornull).
- [`toUInt32OrDefault`](#touint32ordefault).

## toUInt32OrNull

Like [`toUInt32`](#touint32), this function converts an input value to a value of type [UInt32](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toUInt32OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt32OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt32](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 32-bit unsigned integer value if successful, otherwise `NULL`. [UInt32](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero)
, meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt32OrNull('32'),
    toUInt32OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt32OrNull('32'):  32
toUInt32OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toUInt32`](#touint32).
- [`toUInt32OrZero`](#touint32orzero).
- [`toUInt32OrDefault`](#touint32ordefault).

## toUInt32OrDefault

Like [`toUInt32`](#touint32), this function converts an input value to a value of type [UInt32](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toUInt32OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `UInt32` is unsuccessful. [UInt32](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt32OrDefault('0xc0fe', CAST('0', 'UInt32'));`.

:::note
If the input value cannot be represented within the bounds of [UInt32](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 32-bit unsigned integer value if successful, otherwise returns the default value if passed or `0` if not. [UInt32](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toUInt32OrDefault('32', CAST('0', 'UInt32')),
    toUInt32OrDefault('abc', CAST('0', 'UInt32'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt32OrDefault('32', CAST('0', 'UInt32')):  32
toUInt32OrDefault('abc', CAST('0', 'UInt32')): 0
```

**See also**

- [`toUInt32`](#touint32).
- [`toUInt32OrZero`](#touint32orzero).
- [`toUInt32OrNull`](#touint32ornull).

## toUInt64

Converts an input value to a value of type [`UInt64`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toUInt64(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported types:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt64('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt64](../data-types/int-uint.md), the result over or under flows.
This is not considered an error.  
For example: `SELECT toUInt64(18446744073709551616) == 0;`
:::

**Returned value**

- 64-bit unsigned integer value. [UInt64](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toUInt64(64),
    toUInt64(64.64),
    toUInt64('64')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt64(64):    64
toUInt64(64.64): 64
toUInt64('64'):  64
```

**See also**

- [`toUInt64OrZero`](#touint64orzero).
- [`toUInt64OrNull`](#touint64ornull).
- [`toUInt64OrDefault`](#touint64ordefault).

## toInt64OrZero

Like [`toUInt64`](#touint64), this function converts an input value to a value of type [UInt64](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toUInt64OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt64OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt64](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 64-bit unsigned integer value if successful, otherwise `0`. [UInt64](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt64OrZero('64'),
    toUInt64OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt64OrZero('64'):  64
toUInt64OrZero('abc'): 0
```

**See also**

- [`toUInt64`](#touint64).
- [`toUInt64OrNull`](#touint64ornull).
- [`toUInt64OrDefault`](#touint64ordefault).

## toUInt64OrNull

Like [`toUInt64`](#touint64), this function converts an input value to a value of type [UInt64](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toUInt64OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt64OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt64](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 64-bit unsigned integer value if successful, otherwise `NULL`. [UInt64](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt64OrNull('64'),
    toUInt64OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt64OrNull('64'):  64
toUInt64OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toUInt64`](#touint64).
- [`toUInt64OrZero`](#touint64orzero).
- [`toUInt64OrDefault`](#touint64ordefault).

## toUInt64OrDefault

Like [`toUInt64`](#touint64), this function converts an input value to a value of type [UInt64](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toUInt64OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `defauult` (optional) — The default value to return if parsing to type `UInt64` is unsuccessful. [UInt64](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt64OrDefault('0xc0fe', CAST('0', 'UInt64'));`.

:::note
If the input value cannot be represented within the bounds of [UInt64](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 64-bit unsigned integer value if successful, otherwise returns the default value if passed or `0` if not. [UInt64](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toUInt64OrDefault('64', CAST('0', 'UInt64')),
    toUInt64OrDefault('abc', CAST('0', 'UInt64'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt64OrDefault('64', CAST('0', 'UInt64')):  64
toUInt64OrDefault('abc', CAST('0', 'UInt64')): 0
```

**See also**

- [`toUInt64`](#touint64).
- [`toUInt64OrZero`](#touint64orzero).
- [`toUInt64OrNull`](#touint64ornull).

## toUInt128

Converts an input value to a value of type [`UInt128`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toUInt128(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt128('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt128](../data-types/int-uint.md), the result over or under flows.
This is not considered an error.
:::

**Returned value**

- 128-bit unsigned integer value. [UInt128](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toUInt128(128),
    toUInt128(128.8),
    toUInt128('128')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt128(128):   128
toUInt128(128.8): 128
toUInt128('128'): 128
```

**See also**

- [`toUInt128OrZero`](#touint128orzero).
- [`toUInt128OrNull`](#touint128ornull).
- [`toUInt128OrDefault`](#touint128ordefault).

## toUInt128OrZero

Like [`toUInt128`](#touint128), this function converts an input value to a value of type [UInt128](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toUInt128OrZero(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt128OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt128](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 128-bit unsigned integer value if successful, otherwise `0`. [UInt128](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt128OrZero('128'),
    toUInt128OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt128OrZero('128'): 128
toUInt128OrZero('abc'): 0
```

**See also**

- [`toUInt128`](#touint128).
- [`toUInt128OrNull`](#touint128ornull).
- [`toUInt128OrDefault`](#touint128ordefault).

## toUInt128OrNull

Like [`toUInt128`](#touint128), this function converts an input value to a value of type [UInt128](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toUInt128OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt128OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt128](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 128-bit unsigned integer value if successful, otherwise `NULL`. [UInt128](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt128OrNull('128'),
    toUInt128OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt128OrNull('128'): 128
toUInt128OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toUInt128`](#touint128).
- [`toUInt128OrZero`](#touint128orzero).
- [`toUInt128OrDefault`](#touint128ordefault).

## toUInt128OrDefault

Like [`toUInt128`](#toint128), this function converts an input value to a value of type [UInt128](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toUInt128OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `UInt128` is unsuccessful. [UInt128](../data-types/int-uint.md).

Supported arguments:
- (U)Int8/16/32/64/128/256.
- Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt128OrDefault('0xc0fe', CAST('0', 'UInt128'));`.

:::note
If the input value cannot be represented within the bounds of [UInt128](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 128-bit unsigned integer value if successful, otherwise returns the default value if passed or `0` if not. [UInt128](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toUInt128OrDefault('128', CAST('0', 'UInt128')),
    toUInt128OrDefault('abc', CAST('0', 'UInt128'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt128OrDefault('128', CAST('0', 'UInt128')): 128
toUInt128OrDefault('abc', CAST('0', 'UInt128')): 0
```

**See also**

- [`toUInt128`](#touint128).
- [`toUInt128OrZero`](#touint128orzero).
- [`toUInt128OrNull`](#touint128ornull).

## toUInt256

Converts an input value to a value of type [`UInt256`](../data-types/int-uint.md). Throws an exception in case of an error.

**Syntax**

```sql
toUInt256(expr)
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments:
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt256('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt256](../data-types/int-uint.md), the result over or under flows.
This is not considered an error.
:::

**Returned value**

- 256-bit unsigned integer value. [Int256](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

```sql
SELECT
    toUInt256(256),
    toUInt256(256.256),
    toUInt256('256')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt256(256):     256
toUInt256(256.256): 256
toUInt256('256'):   256
```

**See also**

- [`toUInt256OrZero`](#touint256orzero).
- [`toUInt256OrNull`](#touint256ornull).
- [`toUInt256OrDefault`](#touint256ordefault).

## toUInt256OrZero

Like [`toUInt256`](#touint256), this function converts an input value to a value of type [UInt256](../data-types/int-uint.md) but returns `0` in case of an error.

**Syntax**

```sql
toUInt256OrZero(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `0`):
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt256OrZero('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt256](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 256-bit unsigned integer value if successful, otherwise `0`. [UInt256](../data-types/int-uint.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt256OrZero('256'),
    toUInt256OrZero('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt256OrZero('256'): 256
toUInt256OrZero('abc'): 0
```

**See also**

- [`toUInt256`](#touint256).
- [`toUInt256OrNull`](#touint256ornull).
- [`toUInt256OrDefault`](#touint256ordefault).

## toUInt256OrNull

Like [`toUInt256`](#touint256), this function converts an input value to a value of type [UInt256](../data-types/int-uint.md) but returns `NULL` in case of an error.

**Syntax**

```sql
toUInt256OrNull(x)
```

**Arguments**

- `x` — A String representation of a number. [String](../data-types/string.md).

Supported arguments:
- String representations of (U)Int8/16/32/128/256.

Unsupported arguments (return `\N`)
- String representations of Float32/64 values, including `NaN` and `Inf`.
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt256OrNull('0xc0fe');`.

:::note
If the input value cannot be represented within the bounds of [UInt256](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 256-bit unsigned integer value if successful, otherwise `NULL`. [UInt256](../data-types/int-uint.md) / [NULL](../data-types/nullable.md).

:::note
The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
:::

**Example**

Query:

``` sql
SELECT
    toUInt256OrNull('256'),
    toUInt256OrNull('abc')
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt256OrNull('256'): 256
toUInt256OrNull('abc'): ᴺᵁᴸᴸ
```

**See also**

- [`toUInt256`](#touint256).
- [`toUInt256OrZero`](#touint256orzero).
- [`toUInt256OrDefault`](#touint256ordefault).

## toUInt256OrDefault

Like [`toUInt256`](#touint256), this function converts an input value to a value of type [UInt256](../data-types/int-uint.md) but returns the default value in case of an error.
If no `default` value is passed then `0` is returned in case of an error.

**Syntax**

```sql
toUInt256OrDefault(expr[, default])
```

**Arguments**

- `expr` — Expression returning a number or a string representation of a number. [Expression](../syntax.md/#syntax-expressions) / [String](../data-types/string.md).
- `default` (optional) — The default value to return if parsing to type `UInt256` is unsuccessful. [UInt256](../data-types/int-uint.md).

Supported arguments:
- Values of type (U)Int8/16/32/64/128/256.
- Values of type Float32/64.
- String representations of (U)Int8/16/32/128/256.

Arguments for which the default value is returned:
- String representations of Float32/64 values, including `NaN` and `Inf`
- String representations of binary and hexadecimal values, e.g. `SELECT toUInt256OrDefault('0xc0fe', CAST('0', 'UInt256'));`

:::note
If the input value cannot be represented within the bounds of [UInt256](../data-types/int-uint.md), overflow or underflow of the result occurs.
This is not considered an error.
:::

**Returned value**

- 256-bit unsigned integer value if successful, otherwise returns the default value if passed or `0` if not. [UInt256](../data-types/int-uint.md).

:::note
- The function uses [rounding towards zero](https://en.wikipedia.org/wiki/Rounding#Rounding_towards_zero), meaning it truncates fractional digits of numbers.
- The default value type should be the same as the cast type.
:::

**Example**

Query:

``` sql
SELECT
    toUInt256OrDefault('-256', CAST('0', 'UInt256')),
    toUInt256OrDefault('abc', CAST('0', 'UInt256'))
FORMAT vertical;
```

Result:

```response
Row 1:
──────
toUInt256OrDefault('-256', CAST('0', 'UInt256')): 0
toUInt256OrDefault('abc', CAST('0', 'UInt256')):  0
```

**See also**

- [`toUInt256`](#touint256).
- [`toUInt256OrZero`](#touint256orzero).
- [`toUInt256OrNull`](#touint256ornull).

## toFloat(32\|64)

## toFloat(32\|64)OrZero

## toFloat(32\|64)OrNull

## toFloat(32\|64)OrDefault

## toDate

Converts the argument to [Date](../data-types/date.md) data type.

If the argument is [DateTime](../data-types/datetime.md) or [DateTime64](../data-types/datetime64.md), it truncates it and leaves the date component of the DateTime:

```sql
SELECT
    now() AS x,
    toDate(x)
```

```response
┌───────────────────x─┬─toDate(now())─┐
│ 2022-12-30 13:44:17 │    2022-12-30 │
└─────────────────────┴───────────────┘
```

If the argument is a [String](../data-types/string.md), it is parsed as [Date](../data-types/date.md) or [DateTime](../data-types/datetime.md). If it was parsed as [DateTime](../data-types/datetime.md), the date component is being used:

```sql
SELECT
    toDate('2022-12-30') AS x,
    toTypeName(x)
```

```response
┌──────────x─┬─toTypeName(toDate('2022-12-30'))─┐
│ 2022-12-30 │ Date                             │
└────────────┴──────────────────────────────────┘

1 row in set. Elapsed: 0.001 sec.
```

```sql
SELECT
    toDate('2022-12-30 01:02:03') AS x,
    toTypeName(x)
```

```response
┌──────────x─┬─toTypeName(toDate('2022-12-30 01:02:03'))─┐
│ 2022-12-30 │ Date                                      │
└────────────┴───────────────────────────────────────────┘
```

If the argument is a number and looks like a UNIX timestamp (is greater than 65535), it is interpreted as a [DateTime](../data-types/datetime.md), then truncated to [Date](../data-types/date.md) in the current timezone. The timezone argument can be specified as a second argument of the function. The truncation to [Date](../data-types/date.md) depends on the timezone:

```sql
SELECT
    now() AS current_time,
    toUnixTimestamp(current_time) AS ts,
    toDateTime(ts) AS time_Amsterdam,
    toDateTime(ts, 'Pacific/Apia') AS time_Samoa,
    toDate(time_Amsterdam) AS date_Amsterdam,
    toDate(time_Samoa) AS date_Samoa,
    toDate(ts) AS date_Amsterdam_2,
    toDate(ts, 'Pacific/Apia') AS date_Samoa_2
```

```response
Row 1:
──────
current_time:     2022-12-30 13:51:54
ts:               1672404714
time_Amsterdam:   2022-12-30 13:51:54
time_Samoa:       2022-12-31 01:51:54
date_Amsterdam:   2022-12-30
date_Samoa:       2022-12-31
date_Amsterdam_2: 2022-12-30
date_Samoa_2:     2022-12-31
```

The example above demonstrates how the same UNIX timestamp can be interpreted as different dates in different time zones.

If the argument is a number and it is smaller than 65536, it is interpreted as the number of days since 1970-01-01 (the first UNIX day) and converted to [Date](../data-types/date.md). It corresponds to the internal numeric representation of the `Date` data type. Example:

```sql
SELECT toDate(12345)
```
```response
┌─toDate(12345)─┐
│    2003-10-20 │
└───────────────┘
```

This conversion does not depend on timezones.

If the argument does not fit in the range of the Date type, it results in an implementation-defined behavior, that can saturate to the maximum supported date or overflow:
```sql
SELECT toDate(10000000000.)
```
```response
┌─toDate(10000000000.)─┐
│           2106-02-07 │
└──────────────────────┘
```

The function `toDate` can be also written in alternative forms:

```sql
SELECT
    now() AS time,
    toDate(time),
    DATE(time),
    CAST(time, 'Date')
```
```response
┌────────────────time─┬─toDate(now())─┬─DATE(now())─┬─CAST(now(), 'Date')─┐
│ 2022-12-30 13:54:58 │    2022-12-30 │  2022-12-30 │          2022-12-30 │
└─────────────────────┴───────────────┴─────────────┴─────────────────────┘
```


## toDateOrZero

The same as [toDate](#todate) but returns lower boundary of [Date](../data-types/date.md) if an invalid argument is received. Only [String](../data-types/string.md) argument is supported.

**Example**

Query:

``` sql
SELECT toDateOrZero('2022-12-30'), toDateOrZero('');
```

Result:

```response
┌─toDateOrZero('2022-12-30')─┬─toDateOrZero('')─┐
│                 2022-12-30 │       1970-01-01 │
└────────────────────────────┴──────────────────┘
```


## toDateOrNull

The same as [toDate](#todate) but returns `NULL` if an invalid argument is received. Only [String](../data-types/string.md) argument is supported.

**Example**

Query:

``` sql
SELECT toDateOrNull('2022-12-30'), toDateOrNull('');
```

Result:

```response
┌─toDateOrNull('2022-12-30')─┬─toDateOrNull('')─┐
│                 2022-12-30 │             ᴺᵁᴸᴸ │
└────────────────────────────┴──────────────────┘
```


## toDateOrDefault

Like [toDate](#todate) but if unsuccessful, returns a default value which is either the second argument (if specified), or otherwise the lower boundary of [Date](../data-types/date.md).

**Syntax**

``` sql
toDateOrDefault(expr [, default_value])
```

**Example**

Query:

``` sql
SELECT toDateOrDefault('2022-12-30'), toDateOrDefault('', '2023-01-01'::Date);
```

Result:

```response
┌─toDateOrDefault('2022-12-30')─┬─toDateOrDefault('', CAST('2023-01-01', 'Date'))─┐
│                    2022-12-30 │                                      2023-01-01 │
└───────────────────────────────┴─────────────────────────────────────────────────┘
```


## toDateTime

Converts an input value to [DateTime](../data-types/datetime.md).

**Syntax**

``` sql
toDateTime(expr[, time_zone ])
```

**Arguments**

- `expr` — The value. [String](../data-types/string.md), [Int](../data-types/int-uint.md), [Date](../data-types/date.md) or [DateTime](../data-types/datetime.md).
- `time_zone` — Time zone. [String](../data-types/string.md).

:::note
If `expr` is a number, it is interpreted as the number of seconds since the beginning of the Unix Epoch (as Unix timestamp).  
If `expr` is a [String](../data-types/string.md), it may be interpreted as a Unix timestamp or as a string representation of date / date with time.  
Thus, parsing of short numbers' string representations (up to 4 digits) is explicitly disabled due to ambiguity, e.g. a string `'1999'` may be both a year (an incomplete string representation of Date / DateTime) or a unix timestamp. Longer numeric strings are allowed.
:::

**Returned value**

- A date time. [DateTime](../data-types/datetime.md)

**Example**

Query:

``` sql
SELECT toDateTime('2022-12-30 13:44:17'), toDateTime(1685457500, 'UTC');
```

Result:

```response
┌─toDateTime('2022-12-30 13:44:17')─┬─toDateTime(1685457500, 'UTC')─┐
│               2022-12-30 13:44:17 │           2023-05-30 14:38:20 │
└───────────────────────────────────┴───────────────────────────────┘
```


## toDateTimeOrZero

The same as [toDateTime](#todatetime) but returns lower boundary of [DateTime](../data-types/datetime.md) if an invalid argument is received. Only [String](../data-types/string.md) argument is supported.

**Example**

Query:

``` sql
SELECT toDateTimeOrZero('2022-12-30 13:44:17'), toDateTimeOrZero('');
```

Result:

```response
┌─toDateTimeOrZero('2022-12-30 13:44:17')─┬─toDateTimeOrZero('')─┐
│                     2022-12-30 13:44:17 │  1970-01-01 00:00:00 │
└─────────────────────────────────────────┴──────────────────────┘
```


## toDateTimeOrNull

The same as [toDateTime](#todatetime) but returns `NULL` if an invalid argument is received. Only [String](../data-types/string.md) argument is supported.

**Example**

Query:

``` sql
SELECT toDateTimeOrNull('2022-12-30 13:44:17'), toDateTimeOrNull('');
```

Result:

```response
┌─toDateTimeOrNull('2022-12-30 13:44:17')─┬─toDateTimeOrNull('')─┐
│                     2022-12-30 13:44:17 │                 ᴺᵁᴸᴸ │
└─────────────────────────────────────────┴──────────────────────┘
```


## toDateTimeOrDefault

Like [toDateTime](#todatetime) but if unsuccessful, returns a default value which is either the third argument (if specified), or otherwise the lower boundary of [DateTime](../data-types/datetime.md).

**Syntax**

``` sql
toDateTimeOrDefault(expr [, time_zone [, default_value]])
```

**Example**

Query:

``` sql
SELECT toDateTimeOrDefault('2022-12-30 13:44:17'), toDateTimeOrDefault('', 'UTC', '2023-01-01'::DateTime('UTC'));
```

Result:

```response
┌─toDateTimeOrDefault('2022-12-30 13:44:17')─┬─toDateTimeOrDefault('', 'UTC', CAST('2023-01-01', 'DateTime(\'UTC\')'))─┐
│                        2022-12-30 13:44:17 │                                                     2023-01-01 00:00:00 │
└────────────────────────────────────────────┴─────────────────────────────────────────────────────────────────────────┘
```


## toDate32

Converts the argument to the [Date32](../data-types/date32.md) data type. If the value is outside the range, `toDate32` returns the border values supported by [Date32](../data-types/date32.md). If the argument has [Date](../data-types/date.md) type, it's borders are taken into account.

**Syntax**

``` sql
toDate32(expr)
```

**Arguments**

- `expr` — The value. [String](../data-types/string.md), [UInt32](../data-types/int-uint.md) or [Date](../data-types/date.md).

**Returned value**

- A calendar date. Type [Date32](../data-types/date32.md).

**Example**

1. The value is within the range:

``` sql
SELECT toDate32('1955-01-01') AS value, toTypeName(value);
```

```response
┌──────value─┬─toTypeName(toDate32('1925-01-01'))─┐
│ 1955-01-01 │ Date32                             │
└────────────┴────────────────────────────────────┘
```

2. The value is outside the range:

``` sql
SELECT toDate32('1899-01-01') AS value, toTypeName(value);
```

```response
┌──────value─┬─toTypeName(toDate32('1899-01-01'))─┐
│ 1900-01-01 │ Date32                             │
└────────────┴────────────────────────────────────┘
```

3. With [Date](../data-types/date.md) argument:

``` sql
SELECT toDate32(toDate('1899-01-01')) AS value, toTypeName(value);
```

```response
┌──────value─┬─toTypeName(toDate32(toDate('1899-01-01')))─┐
│ 1970-01-01 │ Date32                                     │
└────────────┴────────────────────────────────────────────┘
```

## toDate32OrZero

The same as [toDate32](#todate32) but returns the min value of [Date32](../data-types/date32.md) if an invalid argument is received.

**Example**

Query:

``` sql
SELECT toDate32OrZero('1899-01-01'), toDate32OrZero('');
```

Result:

```response
┌─toDate32OrZero('1899-01-01')─┬─toDate32OrZero('')─┐
│                   1900-01-01 │         1900-01-01 │
└──────────────────────────────┴────────────────────┘
```

## toDate32OrNull

The same as [toDate32](#todate32) but returns `NULL` if an invalid argument is received.

**Example**

Query:

``` sql
SELECT toDate32OrNull('1955-01-01'), toDate32OrNull('');
```

Result:

```response
┌─toDate32OrNull('1955-01-01')─┬─toDate32OrNull('')─┐
│                   1955-01-01 │               ᴺᵁᴸᴸ │
└──────────────────────────────┴────────────────────┘
```

## toDate32OrDefault

Converts the argument to the [Date32](../data-types/date32.md) data type. If the value is outside the range, `toDate32OrDefault` returns the lower border value supported by [Date32](../data-types/date32.md). If the argument has [Date](../data-types/date.md) type, it's borders are taken into account. Returns default value if an invalid argument is received.

**Example**

Query:

``` sql
SELECT
    toDate32OrDefault('1930-01-01', toDate32('2020-01-01')),
    toDate32OrDefault('xx1930-01-01', toDate32('2020-01-01'));
```

Result:

```response
┌─toDate32OrDefault('1930-01-01', toDate32('2020-01-01'))─┬─toDate32OrDefault('xx1930-01-01', toDate32('2020-01-01'))─┐
│                                              1930-01-01 │                                                2020-01-01 │
└─────────────────────────────────────────────────────────┴───────────────────────────────────────────────────────────┘
```

## toDateTime64

Converts the argument to the [DateTime64](../data-types/datetime64.md) data type.

**Syntax**

``` sql
toDateTime64(expr, scale, [timezone])
```

**Arguments**

- `expr` — The value. [String](../data-types/string.md), [UInt32](../data-types/int-uint.md), [Float](../data-types/float.md) or [DateTime](../data-types/datetime.md).
- `scale` - Tick size (precision): 10<sup>-precision</sup> seconds. Valid range: [ 0 : 9 ].
- `timezone` - Time zone of the specified datetime64 object.

**Returned value**

- A calendar date and time of day, with sub-second precision. [DateTime64](../data-types/datetime64.md).

**Example**

1. The value is within the range:

``` sql
SELECT toDateTime64('1955-01-01 00:00:00.000', 3) AS value, toTypeName(value);
```

```response
┌───────────────────value─┬─toTypeName(toDateTime64('1955-01-01 00:00:00.000', 3))─┐
│ 1955-01-01 00:00:00.000 │ DateTime64(3)                                          │
└─────────────────────────┴────────────────────────────────────────────────────────┘
```

2. As decimal with precision:

``` sql
SELECT toDateTime64(1546300800.000, 3) AS value, toTypeName(value);
```

```response
┌───────────────────value─┬─toTypeName(toDateTime64(1546300800., 3))─┐
│ 2019-01-01 00:00:00.000 │ DateTime64(3)                            │
└─────────────────────────┴──────────────────────────────────────────┘
```

Without the decimal point the value is still treated as Unix Timestamp in seconds:

``` sql
SELECT toDateTime64(1546300800000, 3) AS value, toTypeName(value);
```

```response
┌───────────────────value─┬─toTypeName(toDateTime64(1546300800000, 3))─┐
│ 2282-12-31 00:00:00.000 │ DateTime64(3)                              │
└─────────────────────────┴────────────────────────────────────────────┘
```


3. With `timezone`:

``` sql
SELECT toDateTime64('2019-01-01 00:00:00', 3, 'Asia/Istanbul') AS value, toTypeName(value);
```

```response
┌───────────────────value─┬─toTypeName(toDateTime64('2019-01-01 00:00:00', 3, 'Asia/Istanbul'))─┐
│ 2019-01-01 00:00:00.000 │ DateTime64(3, 'Asia/Istanbul')                                      │
└─────────────────────────┴─────────────────────────────────────────────────────────────────────┘
```

## toDateTime64OrZero

## toDateTime64OrNull

## toDateTime64OrDefault

## toDecimal(32\|64\|128\|256)

Converts `value` to the [Decimal](../data-types/decimal.md) data type with precision of `S`. The `value` can be a number or a string. The `S` (scale) parameter specifies the number of decimal places.

- `toDecimal32(value, S)`
- `toDecimal64(value, S)`
- `toDecimal128(value, S)`
- `toDecimal256(value, S)`

## toDecimal(32\|64\|128\|256)OrNull

Converts an input string to a [Nullable(Decimal(P,S))](../data-types/decimal.md) data type value. This family of functions includes:

- `toDecimal32OrNull(expr, S)` — Results in `Nullable(Decimal32(S))` data type.
- `toDecimal64OrNull(expr, S)` — Results in `Nullable(Decimal64(S))` data type.
- `toDecimal128OrNull(expr, S)` — Results in `Nullable(Decimal128(S))` data type.
- `toDecimal256OrNull(expr, S)` — Results in `Nullable(Decimal256(S))` data type.

These functions should be used instead of `toDecimal*()` functions, if you prefer to get a `NULL` value instead of an exception in the event of an input value parsing error.

**Arguments**

- `expr` — [Expression](../syntax.md/#syntax-expressions), returns a value in the [String](../data-types/string.md) data type. ClickHouse expects the textual representation of the decimal number. For example, `'1.111'`.
- `S` — Scale, the number of decimal places in the resulting value.

**Returned value**

A value in the `Nullable(Decimal(P,S))` data type. The value contains:

- Number with `S` decimal places, if ClickHouse interprets the input string as a number.
- `NULL`, if ClickHouse can’t interpret the input string as a number or if the input number contains more than `S` decimal places.

**Examples**

Query:

``` sql
SELECT toDecimal32OrNull(toString(-1.111), 5) AS val, toTypeName(val);
```

Result:

```response
┌────val─┬─toTypeName(toDecimal32OrNull(toString(-1.111), 5))─┐
│ -1.111 │ Nullable(Decimal(9, 5))                            │
└────────┴────────────────────────────────────────────────────┘
```

Query:

``` sql
SELECT toDecimal32OrNull(toString(-1.111), 2) AS val, toTypeName(val);
```

Result:

```response
┌──val─┬─toTypeName(toDecimal32OrNull(toString(-1.111), 2))─┐
│ ᴺᵁᴸᴸ │ Nullable(Decimal(9, 2))                            │
└──────┴────────────────────────────────────────────────────┘
```


## toDecimal(32\|64\|128\|256)OrDefault

Converts an input string to a [Decimal(P,S)](../data-types/decimal.md) data type value. This family of functions includes:

- `toDecimal32OrDefault(expr, S)` — Results in `Decimal32(S)` data type.
- `toDecimal64OrDefault(expr, S)` — Results in `Decimal64(S)` data type.
- `toDecimal128OrDefault(expr, S)` — Results in `Decimal128(S)` data type.
- `toDecimal256OrDefault(expr, S)` — Results in `Decimal256(S)` data type.

These functions should be used instead of `toDecimal*()` functions, if you prefer to get a default value instead of an exception in the event of an input value parsing error.

**Arguments**

- `expr` — [Expression](../syntax.md/#syntax-expressions), returns a value in the [String](../data-types/string.md) data type. ClickHouse expects the textual representation of the decimal number. For example, `'1.111'`.
- `S` — Scale, the number of decimal places in the resulting value.

**Returned value**

A value in the `Decimal(P,S)` data type. The value contains:

- Number with `S` decimal places, if ClickHouse interprets the input string as a number.
- Default `Decimal(P,S)` data type value, if ClickHouse can’t interpret the input string as a number or if the input number contains more than `S` decimal places.

**Examples**

Query:

``` sql
SELECT toDecimal32OrDefault(toString(-1.111), 5) AS val, toTypeName(val);
```

Result:

```response
┌────val─┬─toTypeName(toDecimal32OrDefault(toString(-1.111), 5))─┐
│ -1.111 │ Decimal(9, 5)                                         │
└────────┴───────────────────────────────────────────────────────┘
```

Query:

``` sql
SELECT toDecimal32OrDefault(toString(-1.111), 2) AS val, toTypeName(val);
```

Result:

```response
┌─val─┬─toTypeName(toDecimal32OrDefault(toString(-1.111), 2))─┐
│   0 │ Decimal(9, 2)                                         │
└─────┴───────────────────────────────────────────────────────┘
```

## toDecimal(32\|64\|128\|256)OrZero

Converts an input value to the [Decimal(P,S)](../data-types/decimal.md) data type. This family of functions includes:

- `toDecimal32OrZero( expr, S)` — Results in `Decimal32(S)` data type.
- `toDecimal64OrZero( expr, S)` — Results in `Decimal64(S)` data type.
- `toDecimal128OrZero( expr, S)` — Results in `Decimal128(S)` data type.
- `toDecimal256OrZero( expr, S)` — Results in `Decimal256(S)` data type.

These functions should be used instead of `toDecimal*()` functions, if you prefer to get a `0` value instead of an exception in the event of an input value parsing error.

**Arguments**

- `expr` — [Expression](../syntax.md/#syntax-expressions), returns a value in the [String](../data-types/string.md) data type. ClickHouse expects the textual representation of the decimal number. For example, `'1.111'`.
- `S` — Scale, the number of decimal places in the resulting value.

**Returned value**

A value in the `Nullable(Decimal(P,S))` data type. The value contains:

- Number with `S` decimal places, if ClickHouse interprets the input string as a number.
- 0 with `S` decimal places, if ClickHouse can’t interpret the input string as a number or if the input number contains more than `S` decimal places.

**Example**

Query:

``` sql
SELECT toDecimal32OrZero(toString(-1.111), 5) AS val, toTypeName(val);
```

Result:

```response
┌────val─┬─toTypeName(toDecimal32OrZero(toString(-1.111), 5))─┐
│ -1.111 │ Decimal(9, 5)                                      │
└────────┴────────────────────────────────────────────────────┘
```

Query:

``` sql
SELECT toDecimal32OrZero(toString(-1.111), 2) AS val, toTypeName(val);
```

Result:

```response
┌──val─┬─toTypeName(toDecimal32OrZero(toString(-1.111), 2))─┐
│ 0.00 │ Decimal(9, 2)                                      │
└──────┴────────────────────────────────────────────────────┘
```

## toString

Functions for converting between numbers, strings (but not fixed strings), dates, and dates with times.
All these functions accept one argument.

When converting to or from a string, the value is formatted or parsed using the same rules as for the TabSeparated format (and almost all other text formats). If the string can’t be parsed, an exception is thrown and the request is canceled.

When converting dates to numbers or vice versa, the date corresponds to the number of days since the beginning of the Unix epoch.
When converting dates with times to numbers or vice versa, the date with time corresponds to the number of seconds since the beginning of the Unix epoch.

The date and date-with-time formats for the toDate/toDateTime functions are defined as follows:

```response
YYYY-MM-DD
YYYY-MM-DD hh:mm:ss
```

As an exception, if converting from UInt32, Int32, UInt64, or Int64 numeric types to Date, and if the number is greater than or equal to 65536, the number is interpreted as a Unix timestamp (and not as the number of days) and is rounded to the date. This allows support for the common occurrence of writing `toDate(unix_timestamp)`, which otherwise would be an error and would require writing the more cumbersome `toDate(toDateTime(unix_timestamp))`.

Conversion between a date and a date with time is performed the natural way: by adding a null time or dropping the time.

Conversion between numeric types uses the same rules as assignments between different numeric types in C++.

Additionally, the toString function of the DateTime argument can take a second String argument containing the name of the time zone. Example: `Asia/Yekaterinburg` In this case, the time is formatted according to the specified time zone.

**Example**

Query:

``` sql
SELECT
    now() AS ts,
    time_zone,
    toString(ts, time_zone) AS str_tz_datetime
FROM system.time_zones
WHERE time_zone LIKE 'Europe%'
LIMIT 10
```

Result:

```response
┌──────────────────ts─┬─time_zone─────────┬─str_tz_datetime─────┐
│ 2023-09-08 19:14:59 │ Europe/Amsterdam  │ 2023-09-08 21:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Andorra    │ 2023-09-08 21:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Astrakhan  │ 2023-09-08 23:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Athens     │ 2023-09-08 22:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Belfast    │ 2023-09-08 20:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Belgrade   │ 2023-09-08 21:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Berlin     │ 2023-09-08 21:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Bratislava │ 2023-09-08 21:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Brussels   │ 2023-09-08 21:14:59 │
│ 2023-09-08 19:14:59 │ Europe/Bucharest  │ 2023-09-08 22:14:59 │
└─────────────────────┴───────────────────┴─────────────────────┘
```

Also see the `toUnixTimestamp` function.

## toFixedString(s, N)

Converts a [String](../data-types/string.md) type argument to a [FixedString(N)](../data-types/fixedstring.md) type (a string of fixed length N).
If the string has fewer bytes than N, it is padded with null bytes to the right. If the string has more bytes than N, an exception is thrown.

## toStringCutToZero(s)

Accepts a String or FixedString argument. Returns the String with the content truncated at the first zero byte found.

**Example**

Query:

``` sql
SELECT toFixedString('foo', 8) AS s, toStringCutToZero(s) AS s_cut;
```

Result:

```response
┌─s─────────────┬─s_cut─┐
│ foo\0\0\0\0\0 │ foo   │
└───────────────┴───────┘
```

Query:

``` sql
SELECT toFixedString('foo\0bar', 8) AS s, toStringCutToZero(s) AS s_cut;
```

Result:

```response
┌─s──────────┬─s_cut─┐
│ foo\0bar\0 │ foo   │
└────────────┴───────┘
```

## toDecimalString

Converts a numeric value to String with the number of fractional digits in the output specified by the user.

**Syntax**

``` sql
toDecimalString(number, scale)
```

**Arguments**

- `number` — Value to be represented as String, [Int, UInt](../data-types/int-uint.md), [Float](../data-types/float.md), [Decimal](../data-types/decimal.md),
- `scale` — Number of fractional digits, [UInt8](../data-types/int-uint.md).
    * Maximum scale for [Decimal](../data-types/decimal.md) and [Int, UInt](../data-types/int-uint.md) types is 77 (it is the maximum possible number of significant digits for Decimal),
    * Maximum scale for [Float](../data-types/float.md) is 60.

**Returned value**

- Input value represented as [String](../data-types/string.md) with given number of fractional digits (scale).
    The number is rounded up or down according to common arithmetic in case requested scale is smaller than original number's scale.

**Example**

Query:

``` sql
SELECT toDecimalString(CAST('64.32', 'Float64'), 5);
```

Result:

```response
┌toDecimalString(CAST('64.32', 'Float64'), 5)─┐
│ 64.32000                                    │
└─────────────────────────────────────────────┘
```

## reinterpretAsUInt8

Performs byte reinterpretation by treating the input value as a value of type UInt8. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsUInt8(x)
```

**Parameters**

- `x`: value to byte reinterpret as UInt8. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as UInt8. [UInt8](../data-types/int-uint.md/#uint8-uint16-uint32-uint64-uint128-uint256-int8-int16-int32-int64-int128-int256).

**Example**

Query:

```sql
SELECT
    toInt8(257) AS x,
    toTypeName(x),
    reinterpretAsUInt8(x) AS res,
    toTypeName(res);
```

Result:

```response
┌─x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 1 │ Int8          │   1 │ UInt8           │
└───┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsUInt16

Performs byte reinterpretation by treating the input value as a value of type UInt16. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsUInt16(x)
```

**Parameters**

- `x`: value to byte reinterpret as UInt16. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as UInt16. [UInt16](../data-types/int-uint.md/#uint8-uint16-uint32-uint64-uint128-uint256-int8-int16-int32-int64-int128-int256).

**Example**

Query:

```sql
SELECT
    toUInt8(257) AS x,
    toTypeName(x),
    reinterpretAsUInt16(x) AS res,
    toTypeName(res);
```

Result:

```response
┌─x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 1 │ UInt8         │   1 │ UInt16          │
└───┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsUInt32

Performs byte reinterpretation by treating the input value as a value of type UInt32. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless.

**Syntax**

```sql
reinterpretAsUInt32(x)
```

**Parameters**

- `x`: value to byte reinterpret as UInt32. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as UInt32. [UInt32](../data-types/int-uint.md/#uint8-uint16-uint32-uint64-uint128-uint256-int8-int16-int32-int64-int128-int256).

**Example**

Query:

```sql
SELECT
    toUInt16(257) AS x,
    toTypeName(x),
    reinterpretAsUInt32(x) AS res,
    toTypeName(res)
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ UInt16        │ 257 │ UInt32          │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsUInt64

Performs byte reinterpretation by treating the input value as a value of type UInt64. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsUInt64(x)
```

**Parameters**

- `x`: value to byte reinterpret as UInt64. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as UInt64. [UInt64](../data-types/int-uint.md/#uint8-uint16-uint32-uint64-uint128-uint256-int8-int16-int32-int64-int128-int256).

**Example**

Query:

```sql
SELECT
    toUInt32(257) AS x,
    toTypeName(x),
    reinterpretAsUInt64(x) AS res,
    toTypeName(res)
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ UInt32        │ 257 │ UInt64          │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsUInt128

Performs byte reinterpretation by treating the input value as a value of type UInt128. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsUInt128(x)
```

**Parameters**

- `x`: value to byte reinterpret as UInt128. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as UInt128. [UInt128](../data-types/int-uint.md/#uint8-uint16-uint32-uint64-uint128-uint256-int8-int16-int32-int64-int128-int256).

**Example**

Query:

```sql
SELECT
    toUInt64(257) AS x,
    toTypeName(x),
    reinterpretAsUInt128(x) AS res,
    toTypeName(res)
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ UInt64        │ 257 │ UInt128         │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsUInt256

Performs byte reinterpretation by treating the input value as a value of type UInt256. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless.  

**Syntax**

```sql
reinterpretAsUInt256(x)
```

**Parameters**

- `x`: value to byte reinterpret as UInt256. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as UInt256. [UInt256](../data-types/int-uint.md/#uint8-uint16-uint32-uint64-uint128-uint256-int8-int16-int32-int64-int128-int256).

**Example**

Query:

```sql
SELECT
    toUInt128(257) AS x,
    toTypeName(x),
    reinterpretAsUInt256(x) AS res,
    toTypeName(res)
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ UInt128       │ 257 │ UInt256         │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsInt8

Performs byte reinterpretation by treating the input value as a value of type Int8. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsInt8(x)
```

**Parameters**

- `x`: value to byte reinterpret as Int8. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Int8. [Int8](../data-types/int-uint.md/#int-ranges).

**Example**

Query:

```sql
SELECT
    toUInt8(257) AS x,
    toTypeName(x),
    reinterpretAsInt8(x) AS res,
    toTypeName(res);
```

Result:

```response
┌─x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 1 │ UInt8         │   1 │ Int8            │
└───┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsInt16

Performs byte reinterpretation by treating the input value as a value of type Int16. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless.  

**Syntax**

```sql
reinterpretAsInt16(x)
```

**Parameters**

- `x`: value to byte reinterpret as Int16. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Int16. [Int16](../data-types/int-uint.md/#int-ranges).

**Example**

Query:

```sql
SELECT
    toInt8(257) AS x,
    toTypeName(x),
    reinterpretAsInt16(x) AS res,
    toTypeName(res);
```

Result:

```response
┌─x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 1 │ Int8          │   1 │ Int16           │
└───┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsInt32

Performs byte reinterpretation by treating the input value as a value of type Int32. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsInt32(x)
```

**Parameters**

- `x`: value to byte reinterpret as Int32. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Int32. [Int32](../data-types/int-uint.md/#int-ranges).

**Example**

Query:

```sql
SELECT
    toInt16(257) AS x,
    toTypeName(x),
    reinterpretAsInt32(x) AS res,
    toTypeName(res);
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ Int16         │ 257 │ Int32           │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsInt64

Performs byte reinterpretation by treating the input value as a value of type Int64. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsInt64(x)
```

**Parameters**

- `x`: value to byte reinterpret as Int64. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Int64. [Int64](../data-types/int-uint.md/#int-ranges).

**Example**

Query:

```sql
SELECT
    toInt32(257) AS x,
    toTypeName(x),
    reinterpretAsInt64(x) AS res,
    toTypeName(res);
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ Int32         │ 257 │ Int64           │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsInt128

Performs byte reinterpretation by treating the input value as a value of type Int128. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsInt128(x)
```

**Parameters**

- `x`: value to byte reinterpret as Int128. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Int128. [Int128](../data-types/int-uint.md/#int-ranges).

**Example**

Query:

```sql
SELECT
    toInt64(257) AS x,
    toTypeName(x),
    reinterpretAsInt128(x) AS res,
    toTypeName(res);
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ Int64         │ 257 │ Int128          │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsInt256

Performs byte reinterpretation by treating the input value as a value of type Int256. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsInt256(x)
```

**Parameters**

- `x`: value to byte reinterpret as Int256. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Int256. [Int256](../data-types/int-uint.md/#int-ranges).

**Example**

Query:

```sql
SELECT
    toInt128(257) AS x,
    toTypeName(x),
    reinterpretAsInt256(x) AS res,
    toTypeName(res);
```

Result:

```response
┌───x─┬─toTypeName(x)─┬─res─┬─toTypeName(res)─┐
│ 257 │ Int128        │ 257 │ Int256          │
└─────┴───────────────┴─────┴─────────────────┘
```

## reinterpretAsFloat32

Performs byte reinterpretation by treating the input value as a value of type Float32. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsFloat32(x)
```

**Parameters**

- `x`: value to reinterpret as Float32. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Float32. [Float32](../data-types/float.md).

**Example**

Query:

```sql
SELECT reinterpretAsUInt32(toFloat32(0.2)) as x, reinterpretAsFloat32(x);
```

Result:

```response
┌──────────x─┬─reinterpretAsFloat32(x)─┐
│ 1045220557 │                     0.2 │
└────────────┴─────────────────────────┘
```

## reinterpretAsFloat64

Performs byte reinterpretation by treating the input value as a value of type Float64. Unlike [`CAST`](#cast), the function does not attempt to preserve the original value - if the target type is not able to represent the input type, the output is meaningless. 

**Syntax**

```sql
reinterpretAsFloat64(x)
```

**Parameters**

- `x`: value to reinterpret as Float64. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Reinterpreted value `x` as Float64. [Float64](../data-types/float.md).

**Example**

Query:

```sql
SELECT reinterpretAsUInt64(toFloat64(0.2)) as x, reinterpretAsFloat64(x);
```

Result:

```response
┌───────────────────x─┬─reinterpretAsFloat64(x)─┐
│ 4596373779694328218 │                     0.2 │
└─────────────────────┴─────────────────────────┘
```

## reinterpretAsDate

Accepts a string, fixed string or numeric value and interprets the bytes as a number in host order (little endian). It returns a date from the interpreted number as the number of days since the beginning of the Unix Epoch.

**Syntax**

```sql
reinterpretAsDate(x)
```

**Parameters**

- `x`: number of days since the beginning of the Unix Epoch. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Date. [Date](../data-types/date.md).

**Implementation details**

:::note
If the provided string isn’t long enough, the function works as if the string is padded with the necessary number of null bytes. If the string is longer than needed, the extra bytes are ignored. 
:::

**Example**

Query:

```sql
SELECT reinterpretAsDate(65), reinterpretAsDate('A');
```

Result:

```response
┌─reinterpretAsDate(65)─┬─reinterpretAsDate('A')─┐
│            1970-03-07 │             1970-03-07 │
└───────────────────────┴────────────────────────┘
```

## reinterpretAsDateTime

These functions accept a string and interpret the bytes placed at the beginning of the string as a number in host order (little endian). Returns a date with time interpreted as the number of seconds since the beginning of the Unix Epoch.

**Syntax**

```sql
reinterpretAsDateTime(x)
```

**Parameters**

- `x`: number of seconds since the beginning of the Unix Epoch. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md), [UUID](../data-types/uuid.md), [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

**Returned value**

- Date and Time. [DateTime](../data-types/datetime.md).

**Implementation details**

:::note
If the provided string isn’t long enough, the function works as if the string is padded with the necessary number of null bytes. If the string is longer than needed, the extra bytes are ignored. 
:::

**Example**

Query:

```sql
SELECT reinterpretAsDateTime(65), reinterpretAsDateTime('A');
```

Result:

```response
┌─reinterpretAsDateTime(65)─┬─reinterpretAsDateTime('A')─┐
│       1970-01-01 01:01:05 │        1970-01-01 01:01:05 │
└───────────────────────────┴────────────────────────────┘
```

## reinterpretAsString

This function accepts a number, date or date with time and returns a string containing bytes representing the corresponding value in host order (little endian). Null bytes are dropped from the end. For example, a UInt32 type value of 255 is a string that is one byte long.

**Syntax**

```sql
reinterpretAsString(x)
```

**Parameters**

- `x`: value to reinterpret to string. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md).

**Returned value**

- String containing bytes representing `x`. [String](../data-types/fixedstring.md).

**Example**

Query:

```sql
SELECT 
    reinterpretAsString(toDateTime('1970-01-01 01:01:05')), 
    reinterpretAsString(toDate('1970-03-07'));
```

Result:

```response
┌─reinterpretAsString(toDateTime('1970-01-01 01:01:05'))─┬─reinterpretAsString(toDate('1970-03-07'))─┐
│ A                                                      │ A                                         │
└────────────────────────────────────────────────────────┴───────────────────────────────────────────┘
```

## reinterpretAsFixedString

This function accepts a number, date or date with time and returns a FixedString containing bytes representing the corresponding value in host order (little endian). Null bytes are dropped from the end. For example, a UInt32 type value of 255 is a FixedString that is one byte long.

**Syntax**

```sql
reinterpretAsFixedString(x)
```

**Parameters**

- `x`: value to reinterpret to string. [(U)Int*](../data-types/int-uint.md), [Float](../data-types/float.md), [Date](../data-types/date.md), [DateTime](../data-types/datetime.md).

**Returned value**

- Fixed string containing bytes representing `x`. [FixedString](../data-types/fixedstring.md).

**Example**

Query:

```sql
SELECT 
    reinterpretAsFixedString(toDateTime('1970-01-01 01:01:05')), 
    reinterpretAsFixedString(toDate('1970-03-07'));
```

Result:

```response
┌─reinterpretAsFixedString(toDateTime('1970-01-01 01:01:05'))─┬─reinterpretAsFixedString(toDate('1970-03-07'))─┐
│ A                                                           │ A                                              │
└─────────────────────────────────────────────────────────────┴────────────────────────────────────────────────┘
```

## reinterpretAsUUID

:::note
In addition to the UUID functions listed here, there is dedicated [UUID function documentation](../functions/uuid-functions.md).
:::

Accepts a 16 byte string and returns a UUID containing bytes representing the corresponding value in network byte order (big-endian). If the string isn't long enough, the function works as if the string is padded with the necessary number of null bytes to the end. If the string is longer than 16 bytes, the extra bytes at the end are ignored.

**Syntax**

``` sql
reinterpretAsUUID(fixed_string)
```

**Arguments**

- `fixed_string` — Big-endian byte string. [FixedString](../data-types/fixedstring.md/#fixedstring).

**Returned value**

- The UUID type value. [UUID](../data-types/uuid.md/#uuid-data-type).

**Examples**

String to UUID.

Query:

``` sql
SELECT reinterpretAsUUID(reverse(unhex('000102030405060708090a0b0c0d0e0f')));
```

Result:

```response
┌─reinterpretAsUUID(reverse(unhex('000102030405060708090a0b0c0d0e0f')))─┐
│                                  08090a0b-0c0d-0e0f-0001-020304050607 │
└───────────────────────────────────────────────────────────────────────┘
```

Going back and forth from String to UUID.

Query:

``` sql
WITH
    generateUUIDv4() AS uuid,
    identity(lower(hex(reverse(reinterpretAsString(uuid))))) AS str,
    reinterpretAsUUID(reverse(unhex(str))) AS uuid2
SELECT uuid = uuid2;
```

Result:

```response
┌─equals(uuid, uuid2)─┐
│                   1 │
└─────────────────────┘
```

## reinterpret

Uses the same source in-memory bytes sequence for `x` value and reinterprets it to destination type.

**Syntax**

``` sql
reinterpret(x, type)
```

**Arguments**

- `x` — Any type.
- `type` — Destination type. [String](../data-types/string.md).

**Returned value**

- Destination type value.

**Examples**

Query:
```sql
SELECT reinterpret(toInt8(-1), 'UInt8') as int_to_uint,
    reinterpret(toInt8(1), 'Float32') as int_to_float,
    reinterpret('1', 'UInt32') as string_to_int;
```

Result:

```
┌─int_to_uint─┬─int_to_float─┬─string_to_int─┐
│         255 │        1e-45 │            49 │
└─────────────┴──────────────┴───────────────┘
```

## CAST

Converts an input value to the specified data type. Unlike the [reinterpret](#reinterpret) function, `CAST` tries to present the same value using the new data type. If the conversion can not be done then an exception is raised.
Several syntax variants are supported.

**Syntax**

``` sql
CAST(x, T)
CAST(x AS t)
x::t
```

**Arguments**

- `x` — A value to convert. May be of any type.
- `T` — The name of the target data type. [String](../data-types/string.md).
- `t` — The target data type.

**Returned value**

- Converted value.

:::note
If the input value does not fit the bounds of the target type, the result overflows. For example, `CAST(-1, 'UInt8')` returns `255`.
:::

**Examples**

Query:

```sql
SELECT
    CAST(toInt8(-1), 'UInt8') AS cast_int_to_uint,
    CAST(1.5 AS Decimal(3,2)) AS cast_float_to_decimal,
    '1'::Int32 AS cast_string_to_int;
```

Result:

```
┌─cast_int_to_uint─┬─cast_float_to_decimal─┬─cast_string_to_int─┐
│              255 │                  1.50 │                  1 │
└──────────────────┴───────────────────────┴────────────────────┘
```

Query:

``` sql
SELECT
    '2016-06-15 23:00:00' AS timestamp,
    CAST(timestamp AS DateTime) AS datetime,
    CAST(timestamp AS Date) AS date,
    CAST(timestamp, 'String') AS string,
    CAST(timestamp, 'FixedString(22)') AS fixed_string;
```

Result:

```response
┌─timestamp───────────┬────────────datetime─┬───────date─┬─string──────────────┬─fixed_string──────────────┐
│ 2016-06-15 23:00:00 │ 2016-06-15 23:00:00 │ 2016-06-15 │ 2016-06-15 23:00:00 │ 2016-06-15 23:00:00\0\0\0 │
└─────────────────────┴─────────────────────┴────────────┴─────────────────────┴───────────────────────────┘
```

Conversion to [FixedString (N)](../data-types/fixedstring.md) only works for arguments of type [String](../data-types/string.md) or [FixedString](../data-types/fixedstring.md).

Type conversion to [Nullable](../data-types/nullable.md) and back is supported.

**Example**

Query:

``` sql
SELECT toTypeName(x) FROM t_null;
```

Result:

```response
┌─toTypeName(x)─┐
│ Int8          │
│ Int8          │
└───────────────┘
```

Query:

``` sql
SELECT toTypeName(CAST(x, 'Nullable(UInt16)')) FROM t_null;
```

Result:

```response
┌─toTypeName(CAST(x, 'Nullable(UInt16)'))─┐
│ Nullable(UInt16)                        │
│ Nullable(UInt16)                        │
└─────────────────────────────────────────┘
```

**See also**

- [cast_keep_nullable](../../operations/settings/settings.md/#cast_keep_nullable) setting

## accurateCast(x, T)

Converts `x` to the `T` data type.

The difference from [cast](#cast) is that `accurateCast` does not allow overflow of numeric types during cast if type value `x` does not fit the bounds of type `T`. For example, `accurateCast(-1, 'UInt8')` throws an exception.

**Example**

Query:

``` sql
SELECT cast(-1, 'UInt8') as uint8;
```

Result:

```response
┌─uint8─┐
│   255 │
└───────┘
```

Query:

```sql
SELECT accurateCast(-1, 'UInt8') as uint8;
```

Result:

```response
Code: 70. DB::Exception: Received from localhost:9000. DB::Exception: Value in column Int8 cannot be safely converted into type UInt8: While processing accurateCast(-1, 'UInt8') AS uint8.
```

## accurateCastOrNull(x, T)

Converts input value `x` to the specified data type `T`. Always returns [Nullable](../data-types/nullable.md) type and returns [NULL](../syntax.md/#null-literal) if the casted value is not representable in the target type.

**Syntax**

```sql
accurateCastOrNull(x, T)
```

**Arguments**

- `x` — Input value.
- `T` — The name of the returned data type.

**Returned value**

- The value, converted to the specified data type `T`.

**Example**

Query:

``` sql
SELECT toTypeName(accurateCastOrNull(5, 'UInt8'));
```

Result:

```response
┌─toTypeName(accurateCastOrNull(5, 'UInt8'))─┐
│ Nullable(UInt8)                            │
└────────────────────────────────────────────┘
```

Query:

``` sql
SELECT
    accurateCastOrNull(-1, 'UInt8') as uint8,
    accurateCastOrNull(128, 'Int8') as int8,
    accurateCastOrNull('Test', 'FixedString(2)') as fixed_string;
```

Result:

```response
┌─uint8─┬─int8─┬─fixed_string─┐
│  ᴺᵁᴸᴸ │ ᴺᵁᴸᴸ │ ᴺᵁᴸᴸ         │
└───────┴──────┴──────────────┘
```


## accurateCastOrDefault(x, T[, default_value])

Converts input value `x` to the specified data type `T`. Returns default type value or `default_value` if specified if the casted value is not representable in the target type.

**Syntax**

```sql
accurateCastOrDefault(x, T)
```

**Arguments**

- `x` — Input value.
- `T` — The name of the returned data type.
- `default_value` — Default value of returned data type.

**Returned value**

- The value converted to the specified data type `T`.

**Example**

Query:

``` sql
SELECT toTypeName(accurateCastOrDefault(5, 'UInt8'));
```

Result:

```response
┌─toTypeName(accurateCastOrDefault(5, 'UInt8'))─┐
│ UInt8                                         │
└───────────────────────────────────────────────┘
```

Query:

``` sql
SELECT
    accurateCastOrDefault(-1, 'UInt8') as uint8,
    accurateCastOrDefault(-1, 'UInt8', 5) as uint8_default,
    accurateCastOrDefault(128, 'Int8') as int8,
    accurateCastOrDefault(128, 'Int8', 5) as int8_default,
    accurateCastOrDefault('Test', 'FixedString(2)') as fixed_string,
    accurateCastOrDefault('Test', 'FixedString(2)', 'Te') as fixed_string_default;
```

Result:

```response
┌─uint8─┬─uint8_default─┬─int8─┬─int8_default─┬─fixed_string─┬─fixed_string_default─┐
│     0 │             5 │    0 │            5 │              │ Te                   │
└───────┴───────────────┴──────┴──────────────┴──────────────┴──────────────────────┘
```

## toInterval(Year\|Quarter\|Month\|Week\|Day\|Hour\|Minute\|Second)

Converts a Number type argument to an [Interval](../data-types/special-data-types/interval.md) data type.

**Syntax**

``` sql
toIntervalSecond(number)
toIntervalMinute(number)
toIntervalHour(number)
toIntervalDay(number)
toIntervalWeek(number)
toIntervalMonth(number)
toIntervalQuarter(number)
toIntervalYear(number)
```

**Arguments**

- `number` — Duration of interval. Positive integer number.

**Returned values**

- The value in `Interval` data type.

**Example**

Query:

``` sql
WITH
    toDate('2019-01-01') AS date,
    INTERVAL 1 WEEK AS interval_week,
    toIntervalWeek(1) AS interval_to_week
SELECT
    date + interval_week,
    date + interval_to_week;
```

Result:

```response
┌─plus(date, interval_week)─┬─plus(date, interval_to_week)─┐
│                2019-01-08 │                   2019-01-08 │
└───────────────────────────┴──────────────────────────────┘
```

## parseDateTime

Converts a [String](../data-types/string.md) to [DateTime](../data-types/datetime.md) according to a [MySQL format string](https://dev.mysql.com/doc/refman/8.0/en/date-and-time-functions.html#function_date-format).

This function is the opposite operation of function [formatDateTime](../functions/date-time-functions.md#date_time_functions-formatDateTime).

**Syntax**

``` sql
parseDateTime(str[, format[, timezone]])
```

**Arguments**

- `str` — The String to be parsed
- `format` — The format string. Optional. `%Y-%m-%d %H:%i:%s` if not specified.
- `timezone` — [Timezone](/docs/en/operations/server-configuration-parameters/settings.md/#server_configuration_parameters-timezone). Optional.

**Returned value(s)**

Returns DateTime values parsed from input string according to a MySQL style format string.

**Supported format specifiers**

All format specifiers listed in [formatDateTime](../functions/date-time-functions.md#date_time_functions-formatDateTime) except:
- %Q: Quarter (1-4)

**Example**

``` sql
SELECT parseDateTime('2021-01-04+23:00:00', '%Y-%m-%d+%H:%i:%s')

┌─parseDateTime('2021-01-04+23:00:00', '%Y-%m-%d+%H:%i:%s')─┐
│                                       2021-01-04 23:00:00 │
└───────────────────────────────────────────────────────────┘
```

Alias: `TO_TIMESTAMP`.

## parseDateTimeOrZero

Same as for [parseDateTime](#parsedatetime) except that it returns zero date when it encounters a date format that cannot be processed.

## parseDateTimeOrNull

Same as for [parseDateTime](#parsedatetime) except that it returns `NULL` when it encounters a date format that cannot be processed.

Alias: `str_to_date`.

## parseDateTimeInJodaSyntax

Similar to [parseDateTime](#parsedatetime), except that the format string is in [Joda](https://joda-time.sourceforge.net/apidocs/org/joda/time/format/DateTimeFormat.html) instead of MySQL syntax.

This function is the opposite operation of function [formatDateTimeInJodaSyntax](../functions/date-time-functions.md#date_time_functions-formatDateTimeInJodaSyntax).

**Syntax**

``` sql
parseDateTimeInJodaSyntax(str[, format[, timezone]])
```

**Arguments**

- `str` — The String to be parsed
- `format` — The format string. Optional. `yyyy-MM-dd HH:mm:ss` if not specified.
- `timezone` — [Timezone](/docs/en/operations/server-configuration-parameters/settings.md/#server_configuration_parameters-timezone). Optional.

**Returned value(s)**

Returns DateTime values parsed from input string according to a Joda style format.

**Supported format specifiers**

All format specifiers listed in [formatDateTimeInJoda](../functions/date-time-functions.md#date_time_functions-formatDateTime) are supported, except:
- S: fraction of second
- z: time zone
- Z: time zone offset/id

**Example**

``` sql
SELECT parseDateTimeInJodaSyntax('2023-02-24 14:53:31', 'yyyy-MM-dd HH:mm:ss', 'Europe/Minsk')

┌─parseDateTimeInJodaSyntax('2023-02-24 14:53:31', 'yyyy-MM-dd HH:mm:ss', 'Europe/Minsk')─┐
│                                                                     2023-02-24 14:53:31 │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

## parseDateTimeInJodaSyntaxOrZero

Same as for [parseDateTimeInJodaSyntax](#parsedatetimeinjodasyntax) except that it returns zero date when it encounters a date format that cannot be processed.

## parseDateTimeInJodaSyntaxOrNull

Same as for [parseDateTimeInJodaSyntax](#parsedatetimeinjodasyntax) except that it returns `NULL` when it encounters a date format that cannot be processed.

## parseDateTimeBestEffort
## parseDateTime32BestEffort

Converts a date and time in the [String](../data-types/string.md) representation to [DateTime](../data-types/datetime.md/#data_type-datetime) data type.

The function parses [ISO 8601](https://en.wikipedia.org/wiki/ISO_8601), [RFC 1123 - 5.2.14 RFC-822 Date and Time Specification](https://tools.ietf.org/html/rfc1123#page-55), ClickHouse’s and some other date and time formats.

**Syntax**

``` sql
parseDateTimeBestEffort(time_string [, time_zone])
```

**Arguments**

- `time_string` — String containing a date and time to convert. [String](../data-types/string.md).
- `time_zone` — Time zone. The function parses `time_string` according to the time zone. [String](../data-types/string.md).

**Supported non-standard formats**

- A string containing 9..10 digit [unix timestamp](https://en.wikipedia.org/wiki/Unix_time).
- A string with a date and a time component: `YYYYMMDDhhmmss`, `DD/MM/YYYY hh:mm:ss`, `DD-MM-YY hh:mm`, `YYYY-MM-DD hh:mm:ss`, etc.
- A string with a date, but no time component: `YYYY`, `YYYYMM`, `YYYY*MM`, `DD/MM/YYYY`, `DD-MM-YY` etc.
- A string with a day and time: `DD`, `DD hh`, `DD hh:mm`. In this case `MM` is substituted by `01`.
- A string that includes the date and time along with time zone offset information: `YYYY-MM-DD hh:mm:ss ±h:mm`, etc. For example, `2020-12-12 17:36:00 -5:00`.
- A [syslog timestamp](https://datatracker.ietf.org/doc/html/rfc3164#section-4.1.2): `Mmm dd hh:mm:ss`. For example, `Jun  9 14:20:32`.

For all of the formats with separator the function parses months names expressed by their full name or by the first three letters of a month name. Examples: `24/DEC/18`, `24-Dec-18`, `01-September-2018`.
If the year is not specified, it is considered to be equal to the current year. If the resulting DateTime happen to be in the future (even by a second after the current moment), then the current year is substituted by the previous year.

**Returned value**

- `time_string` converted to the [DateTime](../data-types/datetime.md) data type.

**Examples**

Query:

``` sql
SELECT parseDateTimeBestEffort('23/10/2020 12:12:57')
AS parseDateTimeBestEffort;
```

Result:

```response
┌─parseDateTimeBestEffort─┐
│     2020-10-23 12:12:57 │
└─────────────────────────┘
```

Query:

``` sql
SELECT parseDateTimeBestEffort('Sat, 18 Aug 2018 07:22:16 GMT', 'Asia/Istanbul')
AS parseDateTimeBestEffort;
```

Result:

```response
┌─parseDateTimeBestEffort─┐
│     2018-08-18 10:22:16 │
└─────────────────────────┘
```

Query:

``` sql
SELECT parseDateTimeBestEffort('1284101485')
AS parseDateTimeBestEffort;
```

Result:

```response
┌─parseDateTimeBestEffort─┐
│     2015-07-07 12:04:41 │
└─────────────────────────┘
```

Query:

``` sql
SELECT parseDateTimeBestEffort('2018-10-23 10:12:12')
AS parseDateTimeBestEffort;
```

Result:

```response
┌─parseDateTimeBestEffort─┐
│     2018-10-23 10:12:12 │
└─────────────────────────┘
```

Query:

``` sql
SELECT toYear(now()) as year, parseDateTimeBestEffort('10 20:19');
```

Result:

```response
┌─year─┬─parseDateTimeBestEffort('10 20:19')─┐
│ 2023 │                 2023-01-10 20:19:00 │
└──────┴─────────────────────────────────────┘
```

Query:

``` sql
WITH
    now() AS ts_now,
    formatDateTime(ts_around, '%b %e %T') AS syslog_arg
SELECT
    ts_now,
    syslog_arg,
    parseDateTimeBestEffort(syslog_arg)
FROM (SELECT arrayJoin([ts_now - 30, ts_now + 30]) AS ts_around);
```

Result:

```response
┌──────────────ts_now─┬─syslog_arg──────┬─parseDateTimeBestEffort(syslog_arg)─┐
│ 2023-06-30 23:59:30 │ Jun 30 23:59:00 │                 2023-06-30 23:59:00 │
│ 2023-06-30 23:59:30 │ Jul  1 00:00:00 │                 2022-07-01 00:00:00 │
└─────────────────────┴─────────────────┴─────────────────────────────────────┘
```

**See also**

- [RFC 1123](https://datatracker.ietf.org/doc/html/rfc1123)
- [toDate](#todate)
- [toDateTime](#todatetime)
- [ISO 8601 announcement by @xkcd](https://xkcd.com/1179/)
- [RFC 3164](https://datatracker.ietf.org/doc/html/rfc3164#section-4.1.2)

## parseDateTimeBestEffortUS

This function behaves like [parseDateTimeBestEffort](#parsedatetimebesteffort) for ISO date formats, e.g. `YYYY-MM-DD hh:mm:ss`, and other date formats where the month and date components can be unambiguously extracted, e.g. `YYYYMMDDhhmmss`, `YYYY-MM`, `DD hh`, or `YYYY-MM-DD hh:mm:ss ±h:mm`. If the month and the date components cannot be unambiguously extracted, e.g. `MM/DD/YYYY`, `MM-DD-YYYY`, or `MM-DD-YY`, it prefers the US date format instead of `DD/MM/YYYY`, `DD-MM-YYYY`, or `DD-MM-YY`. As an exception from the latter, if the month is bigger than 12 and smaller or equal than 31, this function falls back to the behavior of [parseDateTimeBestEffort](#parsedatetimebesteffort), e.g. `15/08/2020` is parsed as `2020-08-15`.

## parseDateTimeBestEffortOrNull
## parseDateTime32BestEffortOrNull

Same as for [parseDateTimeBestEffort](#parsedatetimebesteffort) except that it returns `NULL` when it encounters a date format that cannot be processed.

## parseDateTimeBestEffortOrZero
## parseDateTime32BestEffortOrZero

Same as for [parseDateTimeBestEffort](#parsedatetimebesteffort) except that it returns zero date or zero date time when it encounters a date format that cannot be processed.

## parseDateTimeBestEffortUSOrNull

Same as [parseDateTimeBestEffortUS](#parsedatetimebesteffortus) function except that it returns `NULL` when it encounters a date format that cannot be processed.

## parseDateTimeBestEffortUSOrZero

Same as [parseDateTimeBestEffortUS](#parsedatetimebesteffortus) function except that it returns zero date (`1970-01-01`) or zero date with time (`1970-01-01 00:00:00`) when it encounters a date format that cannot be processed.

## parseDateTime64BestEffort

Same as [parseDateTimeBestEffort](#parsedatetimebesteffort) function but also parse milliseconds and microseconds and returns [DateTime](../functions/type-conversion-functions.md/#data_type-datetime) data type.

**Syntax**

``` sql
parseDateTime64BestEffort(time_string [, precision [, time_zone]])
```

**Arguments**

- `time_string` — String containing a date or date with time to convert. [String](../data-types/string.md).
- `precision` — Required precision. `3` — for milliseconds, `6` — for microseconds. Default — `3`. Optional. [UInt8](../data-types/int-uint.md).
- `time_zone` — [Timezone](/docs/en/operations/server-configuration-parameters/settings.md/#server_configuration_parameters-timezone). The function parses `time_string` according to the timezone. Optional. [String](../data-types/string.md).

**Returned value**

- `time_string` converted to the [DateTime](../data-types/datetime.md) data type.

**Examples**

Query:

```sql
SELECT parseDateTime64BestEffort('2021-01-01') AS a, toTypeName(a) AS t
UNION ALL
SELECT parseDateTime64BestEffort('2021-01-01 01:01:00.12346') AS a, toTypeName(a) AS t
UNION ALL
SELECT parseDateTime64BestEffort('2021-01-01 01:01:00.12346',6) AS a, toTypeName(a) AS t
UNION ALL
SELECT parseDateTime64BestEffort('2021-01-01 01:01:00.12346',3,'Asia/Istanbul') AS a, toTypeName(a) AS t
FORMAT PrettyCompactMonoBlock;
```

Result:

```
┌──────────────────────────a─┬─t──────────────────────────────┐
│ 2021-01-01 01:01:00.123000 │ DateTime64(3)                  │
│ 2021-01-01 00:00:00.000000 │ DateTime64(3)                  │
│ 2021-01-01 01:01:00.123460 │ DateTime64(6)                  │
│ 2020-12-31 22:01:00.123000 │ DateTime64(3, 'Asia/Istanbul') │
└────────────────────────────┴────────────────────────────────┘
```

## parseDateTime64BestEffortUS

Same as for [parseDateTime64BestEffort](#parsedatetime64besteffort), except that this function prefers US date format (`MM/DD/YYYY` etc.) in case of ambiguity.

## parseDateTime64BestEffortOrNull

Same as for [parseDateTime64BestEffort](#parsedatetime64besteffort) except that it returns `NULL` when it encounters a date format that cannot be processed.

## parseDateTime64BestEffortOrZero

Same as for [parseDateTime64BestEffort](#parsedatetime64besteffort) except that it returns zero date or zero date time when it encounters a date format that cannot be processed.

## parseDateTime64BestEffortUSOrNull

Same as for [parseDateTime64BestEffort](#parsedatetime64besteffort), except that this function prefers US date format (`MM/DD/YYYY` etc.) in case of ambiguity and returns `NULL` when it encounters a date format that cannot be processed.

## parseDateTime64BestEffortUSOrZero

Same as for [parseDateTime64BestEffort](#parsedatetime64besteffort), except that this function prefers US date format (`MM/DD/YYYY` etc.) in case of ambiguity and returns zero date or zero date time when it encounters a date format that cannot be processed.

## toLowCardinality

Converts input parameter to the [LowCardinality](../data-types/lowcardinality.md) version of same data type.

To convert data from the `LowCardinality` data type use the [CAST](#cast) function. For example, `CAST(x as String)`.

**Syntax**

```sql
toLowCardinality(expr)
```

**Arguments**

- `expr` — [Expression](../syntax.md/#syntax-expressions) resulting in one of the [supported data types](../data-types/index.md/#data_types).

**Returned values**

- Result of `expr`. [LowCardinality](../data-types/lowcardinality.md) of the type of `expr`. 

**Example**

Query:

```sql
SELECT toLowCardinality('1');
```

Result:

```response
┌─toLowCardinality('1')─┐
│ 1                     │
└───────────────────────┘
```

## toUnixTimestamp64Milli

Converts a `DateTime64` to a `Int64` value with fixed millisecond precision. The input value is scaled up or down appropriately depending on its precision.

:::note
The output value is a timestamp in UTC, not in the timezone of `DateTime64`.
:::

**Syntax**

```sql
toUnixTimestamp64Milli(value)
```

**Arguments**

- `value` — DateTime64 value with any precision. [DateTime64](../data-types/datetime64.md).

**Returned value**

- `value` converted to the `Int64` data type. [Int64](../data-types/int-uint.md).

**Example**

Query:

```sql
WITH toDateTime64('2009-02-13 23:31:31.011', 3, 'UTC') AS dt64
SELECT toUnixTimestamp64Milli(dt64);
```

Result:

```response
┌─toUnixTimestamp64Milli(dt64)─┐
│                1234567891011 │
└──────────────────────────────┘
```

## toUnixTimestamp64Micro

Converts a `DateTime64` to a `Int64` value with fixed microsecond precision. The input value is scaled up or down appropriately depending on its precision.

:::note
The output value is a timestamp in UTC, not in the timezone of `DateTime64`.
:::

**Syntax**

```sql
toUnixTimestamp64Micro(value)
```

**Arguments**

- `value` — DateTime64 value with any precision. [DateTime64](../data-types/datetime64.md).

**Returned value**

- `value` converted to the `Int64` data type. [Int64](../data-types/int-uint.md).

**Example**

Query:

```sql
WITH toDateTime64('1970-01-15 06:56:07.891011', 6, 'UTC') AS dt64
SELECT toUnixTimestamp64Micro(dt64);
```

Result:

```response
┌─toUnixTimestamp64Micro(dt64)─┐
│                1234567891011 │
└──────────────────────────────┘
```

## toUnixTimestamp64Nano

Converts a `DateTime64` to a `Int64` value with fixed nanosecond precision. The input value is scaled up or down appropriately depending on its precision.

:::note
The output value is a timestamp in UTC, not in the timezone of `DateTime64`.
:::

**Syntax**

```sql
toUnixTimestamp64Nano(value)
```

**Arguments**

- `value` — DateTime64 value with any precision. [DateTime64](../data-types/datetime64.md).

**Returned value**

- `value` converted to the `Int64` data type. [Int64](../data-types/int-uint.md).

**Example**

Query:

```sql
WITH toDateTime64('1970-01-01 00:20:34.567891011', 9, 'UTC') AS dt64
SELECT toUnixTimestamp64Nano(dt64);
```

Result:

```response
┌─toUnixTimestamp64Nano(dt64)─┐
│               1234567891011 │
└─────────────────────────────┘
```

## fromUnixTimestamp64Milli

Converts an `Int64` to a `DateTime64` value with fixed millisecond precision and optional timezone. The input value is scaled up or down appropriately depending on its precision.

:::note
Please note that input value is treated as a UTC timestamp, not timestamp at the given (or implicit) timezone.
:::

**Syntax**

``` sql
fromUnixTimestamp64Milli(value[, timezone])
```

**Arguments**

- `value` — value with any precision. [Int64](../data-types/int-uint.md).
- `timezone` — (optional) timezone name of the result. [String](../data-types/string.md).

**Returned value**

- `value` converted to DateTime64 with precision `3`. [DateTime64](../data-types/datetime64.md).

**Example**

Query:

``` sql
WITH CAST(1234567891011, 'Int64') AS i64
SELECT
    fromUnixTimestamp64Milli(i64, 'UTC') AS x,
    toTypeName(x);
```

Result:

```response
┌───────────────────────x─┬─toTypeName(x)────────┐
│ 2009-02-13 23:31:31.011 │ DateTime64(3, 'UTC') │
└─────────────────────────┴──────────────────────┘
```

## fromUnixTimestamp64Micro

Converts an `Int64` to a `DateTime64` value with fixed microsecond precision and optional timezone. The input value is scaled up or down appropriately depending on its precision.

:::note
Please note that input value is treated as a UTC timestamp, not timestamp at the given (or implicit) timezone.
:::

**Syntax**

``` sql
fromUnixTimestamp64Micro(value[, timezone])
```

**Arguments**

- `value` — value with any precision. [Int64](../data-types/int-uint.md).
- `timezone` — (optional) timezone name of the result. [String](../data-types/string.md).

**Returned value**

- `value` converted to DateTime64 with precision `6`. [DateTime64](../data-types/datetime64.md).

**Example**

Query:

``` sql
WITH CAST(1234567891011, 'Int64') AS i64
SELECT
    fromUnixTimestamp64Micro(i64, 'UTC') AS x,
    toTypeName(x);
```

Result:

```response
┌──────────────────────────x─┬─toTypeName(x)────────┐
│ 1970-01-15 06:56:07.891011 │ DateTime64(6, 'UTC') │
└────────────────────────────┴──────────────────────┘
```

## fromUnixTimestamp64Nano

Converts an `Int64` to a `DateTime64` value with fixed nanosecond precision and optional timezone. The input value is scaled up or down appropriately depending on its precision. 

:::note
Please note that input value is treated as a UTC timestamp, not timestamp at the given (or implicit) timezone.
:::

**Syntax**

``` sql
fromUnixTimestamp64Nano(value[, timezone])
```

**Arguments**

- `value` — value with any precision. [Int64](../data-types/int-uint.md).
- `timezone` — (optional) timezone name of the result. [String](../data-types/string.md).

**Returned value**

- `value` converted to DateTime64 with precision `9`. [DateTime64](../data-types/datetime64.md).

**Example**

Query:

``` sql
WITH CAST(1234567891011, 'Int64') AS i64
SELECT
    fromUnixTimestamp64Nano(i64, 'UTC') AS x,
    toTypeName(x);
```

Result:

```response
┌─────────────────────────────x─┬─toTypeName(x)────────┐
│ 1970-01-01 00:20:34.567891011 │ DateTime64(9, 'UTC') │
└───────────────────────────────┴──────────────────────┘
```

## formatRow

Converts arbitrary expressions into a string via given format.

**Syntax**

``` sql
formatRow(format, x, y, ...)
```

**Arguments**

- `format` — Text format. For example, [CSV](/docs/en/interfaces/formats.md/#csv), [TSV](/docs/en/interfaces/formats.md/#tabseparated).
- `x`,`y`, ... — Expressions.

**Returned value**

- A formatted string. (for text formats it's usually terminated with the new line character).

**Example**

Query:

``` sql
SELECT formatRow('CSV', number, 'good')
FROM numbers(3);
```

Result:

```response
┌─formatRow('CSV', number, 'good')─┐
│ 0,"good"
                         │
│ 1,"good"
                         │
│ 2,"good"
                         │
└──────────────────────────────────┘
```

**Note**: If format contains suffix/prefix, it will be written in each row.

**Example**

Query:

``` sql
SELECT formatRow('CustomSeparated', number, 'good')
FROM numbers(3)
SETTINGS format_custom_result_before_delimiter='<prefix>\n', format_custom_result_after_delimiter='<suffix>'
```

Result:

```response
┌─formatRow('CustomSeparated', number, 'good')─┐
│ <prefix>
0	good
<suffix>                   │
│ <prefix>
1	good
<suffix>                   │
│ <prefix>
2	good
<suffix>                   │
└──────────────────────────────────────────────┘
```

Note: Only row-based formats are supported in this function.

## formatRowNoNewline

Converts arbitrary expressions into a string via given format. Differs from formatRow in that this function trims the last `\n` if any.

**Syntax**

``` sql
formatRowNoNewline(format, x, y, ...)
```

**Arguments**

- `format` — Text format. For example, [CSV](/docs/en/interfaces/formats.md/#csv), [TSV](/docs/en/interfaces/formats.md/#tabseparated).
- `x`,`y`, ... — Expressions.

**Returned value**

- A formatted string.

**Example**

Query:

``` sql
SELECT formatRowNoNewline('CSV', number, 'good')
FROM numbers(3);
```

Result:

```response
┌─formatRowNoNewline('CSV', number, 'good')─┐
│ 0,"good"                                  │
│ 1,"good"                                  │
│ 2,"good"                                  │
└───────────────────────────────────────────┘
```
