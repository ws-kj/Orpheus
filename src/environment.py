import obj

class Environment(object):
    def __init__(self, outer=None):
        self.store = {}
        self.outer = outer

    def get(self, name):
        if name in self.store:
            return self.store[name]
        elif self.outer != None:
            return self.outer.get(name)

        return None

    def set(self, name, val):
        self.store[name] = val
        return val
    
