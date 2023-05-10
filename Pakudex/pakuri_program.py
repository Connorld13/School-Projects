

def main():
    pakudex = None

    # Welcome Print Message

    print("Welcome to Pakudex: Tracker Extraordinaire!")

    # Prompt the user for parameters of Pakudex

    capacity = 0
    while capacity <= 0:
        try:
            print("Enter max capacity of the Pakudex: ",end = '')
            capacity = int(input())

            if capacity < 0:
                raise Exception()

        except Exception as e:
            print("Please enter a valid size.")

    print("The Pakudex can hold" + str(capacity) + "species of Pakuri.")
    pakudex = capacity

    continueProgram = True
    while continueProgram:

        # Display the Menu

        print(" ")
        print("Pakudex Main Menu")
        print("-----------------")
        print("1. List Pakuri")
        print("2. Show Pakuri")
        print("3. Add Pakuri")
        print("4. Evolve Pakuri")
        print("5. Sort Pakuri")
        print("6. Exit")
        print(" ")

        # Prompt the user for a menu selection

        print ("What would you like to do? ", end = '')
        selection = -1

        try:
            selection = int(input())
        except Exception as e:

            pass
        if selection < 1 or selection > 6:
            print("Unrecognized menu selection!")

        # List Pakuri status
        if selection == 1:

            speciesArray =pakudex.get_species_array()

            if speciesArray is None:
                print("No Pakuri in Pakudex yet!")
                continue

            print("Pakuri In Pakudex: ")
            pakudexSize = pakudex.get_size()
            for i in range(0, pakudexSize):
                species = speciesArray[i]
                print(str(i + 1) + ". " + species)

        # Sort Pakuri
        elif selection == 2:
            print("Enter the name of the species to display: ", end='')
            species = input()

            stats = pakudex.get_stats(species)

            if stats is None:
                print("Error: No such Pakuri!")
                continue

            print(" ")
            print("Species: " + species)
            print("Attack: " + str(stats[0]))
            print("Defense: " + str(stats[1]))
            print("Speed: " + str(stats[2]))
            print(" ")

        elif selection == 3:

        # Exception if full
            size = pakudex.get_size()
            if size == capacity:
                print("Error: Pakudex is full!")
                continue

            print("Enter the name of the species to add: ", end = '')
            species = input()

            methodAnswer = pakudex.add_pakuri(species)
            if methodAnswer:
                print("Pakuri species "+ species + "successfully added!")
            else:
                print("Error: Pakudex already contains this species!")

        # Evolve Pakuri

        elif selection == 4:
            print("Enter the name of the species to evolve: ", end = '')
            species = input()

            methodAnswer = pakudex.evolve_species(species)
            if methodAnswer:
                print(species + " has evolved!")
            else:
                print("Error: No such Pakuri!")

        # Sort Pakuri

        elif selection == 5:
            pakudex.sort_pakuri()
            print("Pakuri have been sorted!")

        # End response

        elif selection == 6:
            continueProgram = False
            print("Thanks for using Pakudex! Bye!")

if __name__=='__main__':
    main()
pass
