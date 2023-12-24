from dataclasses import dataclass

class Entry:
    def __init__(self, value, tsig):
        self.value = value
        self.type_signature = tsig

class Environment(object):
    def __init__(self, outer=None):
        self.store = {}
        self.outer = outer

    def get(self, name):
        if name in self.store:
            return self.store[name].value
        elif self.outer != None:
            return self.outer.get(name)

        return None

    def set(self, name, val, sig=None):
        if self.outer != None and self.outer.get(name) != None:
            self.outer.set(name, val, sig)
            return val

        self.store[name] = Entry(val, sig)
        return val
