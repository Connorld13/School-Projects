from pakuri import Pakuri

class DuplicateSpeciesError(Exception):
    pass
# custom exception to be raised when a pakuri with the same species already exists
class NoneExistantPakuriError(Exception):
    pass

# custom exception when there is no pakuri with specified species input

class Pakudex:

    def __init__(self, capacity=20):
        self.capacity = capacity
        self.species_obj_arr = []
        self.species_str_arr = []

    def get_size(self):
        return len(self.species_obj_arr)

    def get_capacity(self):
        return self.capacity

    def get_species_array(self):
        if self.get_size() > 0:
            return self.species_str_arr
        else:
            return None

    def get_stats(self, species):
        stats_arr = []
        try:
            if self.get_size() != 0:
                for idx, pakuri in enumerate(self.species_obj_arr[0:]):
                    if pakuri.get_species() == species and (idx + 1) <= self.get_size():
                        stats_arr.append(pakuri.get_attack())
                        stats_arr.append(pakuri.get_defense())
                        stats_arr.append(pakuri.get_speed())
                        return stats_arr
                        break
                    elif idx + 1 == self.get_size():
                        raise NoneExistantPakuriError('Error: No such Pakuri!')
                    else:
                        continue
            else:
                raise NoneExistantPakuriError('Error: No such Pakuri!')
        except NoneExistantPakuriError as exception:
            print(exception)
            return None

    # used to get stats or pakuri doesn't exist

    def sort_pakuri(self):
        if self.get_size() > 0:
            self.species_str_arr.sort()
            print('Pakuri have been sorted!')
        else:
            return None

    # sorts the pakuri array to be listed

    def add_pakuri(self, species):
        try:
            for pakuri in self.species_obj_arr:
                if pakuri.get_species() == species:
                    raise DuplicateSpeciesError('Error: Pakudex already contains this species!')
            if len(self.species_obj_arr) < self.capacity:
                self.species_obj_arr.append(Pakuri(species))
                self.species_str_arr.append(species)
                print(f'Pakuri species {species} successfully added!')
                return True
        except DuplicateSpeciesError as exception:
            print(exception)
            return False

    # adds a pakuri to the pakudex or pakuri already exists in the pakudex

    def evolve_species(self, species):
        try:
            if self.get_size() != 0:
                for idx, pakuri in enumerate(self.species_obj_arr[0:]):
                    if pakuri.get_species() == species and (idx + 1) <= self.get_size():
                        pakuri.evolve()
                        return True
                        break
                    elif idx + 1 == self.get_size():
                        raise NoneExistantPakuriError('Error: No such Pakuri!')
                    else:
                        continue
            else:
                raise NoneExistantPakuriError('Error: No such Pakuri!')
        except NoneExistantPakuriError as exception:
            print(exception)
            return False
