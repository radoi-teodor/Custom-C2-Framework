Here we will define some basic rules that will make our project more organized, and so, easily scalable.
# Wiki Writing
- do not use WikiLinks to link images (`Settings > Files and Links > Use [[WikiLinks]]` (turn off))
- Use relative path for linking files (`Settings > Files and Links > New link format` set on `Relative Path)
- Before and after inserting an image, insert a break-line
- After the last element of a list, insert a break-line (otherwise, the text written after will be appended to last bullet content)
- Don't start a Wiki Page with a h1 (`#`), because the page will be named that header

We will structure our wiki pages using the following categories:
- *teamserver* - everything that runs on the teamserver side can be documented here
- *client* - anything interesting that runs on the client side can be written here
- beacon - anything that is related to how a beacon works can be described here (**ATTENTION:** techniques used can be described in particular in the next section)
- *techniques* - any technique used by teamserver-beacons can be described here
	- *protocols* - any communication protocol that is more complex and needs documentation, can be described here
	- *evasion* - any technique used for evasion of AMSI, ETW, EDR hooks etc. can be described here
	- other cases - this is the category that will swallow anything that does not belong in the other categories

Every time we research a technique, I would suggest the following structure for the research page:
1. What are we trying to achieve (for ex.: *we want to bypass LDAP network traffic inspection on EDR agent level in order to execute ADSearch on the compromised machine*)
2. What is the technique used (for ex.: *we should migrate our process in gpresult.exe in order to bypass network inspections*) - here, we can also draw some diagram if needed
3. Implementation described technically - how we can implement the specific technique
4. Impact - how can a SOC identify what we do? how can we cover our tracks?

**ATTENTION:** For diagrams, I recommend [draw.io](https://draw.io/).
# Content Structure
We have to split our work in multiple categories:
- *teamserver project* - the service that will catch the beacon connections
- *client project* - the program that will run on the operator end and will control the *teamserver features*
- *beacon* - see beacon structure
	- *core* - all the main functionalities
	- *communicator* - the service that communicates with *teamserver* via a specific protocol (HTTP, HTTPS, DNS, SMB etc.) and triggers some actions from beacon-core
- *PoCs* - everytime we research some complex feature, we may need a PoC project just to play around with the feature in order to understand it completly; we will use this directory to host our PoC projects
