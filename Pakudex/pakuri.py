class Pakuri:

        #species needs to be a type: string

        #attack,defense, and speed needs type:integer

    def __init__(self, species):
        self.species = species
        self.attack = (len(species) * 7) + 9
        self.defense = (len(species) * 5) + 17
        self.speed = (len(species) * 6) + 13
    # initialize the pakuri object with species attribute

# returns the species
    def get_species(self):
        return self.species

# returns the attack values

    def get_attack(self):
        return self.attack



    def get_defense(self):
        return self.defense

    # returns the defense value

    def get_speed(self):
        return self.speed
    # returns the speed

    def set_attack(self, new_attack):
        self.attack = new_attack
    # changes the attack values to new_attack

    def evolve(self):
        self.attack = self.attack*2
        self.defense= self.defense*4
        self.speed = self.speed*3
# will evolve