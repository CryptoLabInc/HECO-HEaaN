
from typing import TypeVar, NewType

# Generic types. Those rely on Python not enforcing types.
Secret      = TypeVar("Secret")
NonSecret   = TypeVar("NonSecret")

# Specific types. Those are useful if we want to hint to the translation what value the arguments haves
SecretInt       = NewType("SecretInt", int)
SecretFloat     = NewType("SecretFloat", float)
SecretDouble    = NewType("SecretDouble", float)
SecretBool      = NewType("SecretBool", bool)
SecretString    = NewType("SecretString", str)
SecretChar      = NewType("SecretChar", str)

SecretIntVector     = NewType("SecretIntVector", list[int])
SecretFloatVector   = NewType("SecretFloatVector", list[float])
SecretDoubleVector  = NewType("SecretDoubleVector", list[float])
SecretBoolVector    = NewType("SecretBoolVector", list[bool])
SecretStringVector  = NewType("SecretStringVector", list[str])
SecretCharVector    = NewType("SecretCharVector", list[str])


NonSecretInt    = NewType("NonSecretInt", int)
NonSecretFloat  = NewType("NonSecretFloat", float)
NonSecretDouble = NewType("NonSecretDouble", float)
NonSecretBool   = NewType("NonSecretBool", bool)
NonSecretString = NewType("NonSecretString", str)
NonSecretChar   = NewType("NonSecretChar", str)

NonSecretIntVector      = NewType("NonSecretIntVector", list[int])
NonSecretFloatVector    = NewType("NonSecretFloatVector", list[float])
NonSecretDoubleVector   = NewType("NonSecretDoubleVector", list[float])
NonSecretBoolVector     = NewType("NonSecretBoolVector", list[bool])
NonSecretStringVector   = NewType("NonSecretStringVector", list[str])
NonSecretCharVector     = NewType("NonSecretCharVector", list[str])

def is_secret(val_type):
    """
    Return true when a type is secret. All unknown values default to being secret.

    :param val_type: either a Secret* or NonSecret* type as type or String representation of the type.
                 Any other types default to being secret too.
    :return: false if it is a NonSecret* type (as type or string), true otherwise
    """

    non_secret_types = [NonSecret, NonSecretInt, NonSecretFloat, NonSecretDouble, NonSecretBool, NonSecretString, NonSecretChar, NonSecretIntVector,
                        NonSecretFloatVector, NonSecretDoubleVector, NonSecretBoolVector, NonSecretStringVector, NonSecretCharVector]

    # Case 1: type is given as string, check if it corresponds to the string version of a non_secret type
    if isinstance(val_type, str):
        return val_type not in map(lambda x: x.__name__, non_secret_types)
    # Case 2: type is given as TypeVar or NewType, compare it directly to our non-secret types
    elif isinstance(val_type, type(TypeVar)) or isinstance(val_type, type(NewType)):
        return val_type not in non_secret_types
    # Case 3: any other type just defaults to being secret
    else:
        return True
