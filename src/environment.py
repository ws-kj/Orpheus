import obj

class Environment(object):
    def __init__(self):
        self.store = {}

    def get(self, name):
        if name in self.store:
            return self.store[name]
        return None

    def set(self, name, val):
        self.store[name] = val
        return val
    
