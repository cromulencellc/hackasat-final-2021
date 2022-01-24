import json
import collections
import time

#This code refreshes the scoreboard json file periodically to give the
#appearance of a live game.  Install it with a cron job.


from typing import Optional, Any, Dict, List, TypeVar, Callable, Type, cast


T = TypeVar("T")


def from_int(x: Any) -> int:
    assert isinstance(x, int) and not isinstance(x, bool)
    return x


def from_none(x: Any) -> Any:
    assert x is None
    return x


def from_union(fs, x):
    for f in fs:
        try:
            return f(x)
        except:
            pass
    assert False


def from_float(x: Any) -> float:
    assert isinstance(x, (float, int)) and not isinstance(x, bool)
    return float(x)


def to_float(x: Any) -> float:
    assert isinstance(x, float)
    return x


def from_str(x: Any) -> str:
    assert isinstance(x, str)
    return x


def from_dict(f: Callable[[Any], T], x: Any) -> Dict[str, T]:
    assert isinstance(x, dict)
    return { k: f(v) for (k, v) in x.items() }


def to_class(c: Type[T], x: Any) -> dict:
    assert isinstance(x, c)
    return cast(Any, x).to_dict()


def from_list(f: Callable[[Any], T], x: Any) -> List[T]:
    assert isinstance(x, list)
    return [f(y) for y in x]


class Metrics:
    adcsHappy: Optional[int]
    batterystateofcharge: float
    cndhhappy: int
    cosmoshappy: int
    adcshappy: Optional[int]

    def __init__(self, adcsHappy: Optional[int], batterystateofcharge: float, cndhhappy: int, cosmoshappy: int, adcshappy: Optional[int]) -> None:
        self.adcsHappy = adcsHappy
        self.batterystateofcharge = batterystateofcharge
        self.cndhhappy = cndhhappy
        self.cosmoshappy = cosmoshappy
        self.adcshappy = adcshappy

    @staticmethod
    def from_dict(obj: Any) -> 'Metrics':
        assert isinstance(obj, dict)
        adcsHappy = from_union([from_int, from_none], obj.get("adcs_happy"))
        batterystateofcharge = from_float(obj.get("battery_state_of_charge"))
        cndhhappy = from_int(obj.get("cndh_happy"))
        cosmoshappy = from_int(obj.get("cosmos_happy"))
        adcshappy = from_union([from_int, from_none], obj.get("adcs_happy"))
        return Metrics(adcsHappy, batterystateofcharge, cndhhappy, cosmoshappy, adcshappy)

    def to_dict(self) -> dict:
        result: dict = {}
        result["adcs_happy"] = from_union([from_int, from_none], self.adcsHappy)
        result["battery_state_of_charge"] = to_float(self.batterystateofcharge)
        result["cndh_happy"] = from_int(self.cndhhappy)
        result["cosmos_happy"] = from_int(self.cosmoshappy)
        result["adcs_happy"] = from_union([from_int, from_none], self.adcshappy)
        return result


class Standing:
    id: int
    metrics: Metrics
    name: str
    rank: int
    score: int
    scorehistory: Dict[str, int]

    def __init__(self, id: int, metrics: Metrics, name: str, rank: int, score: int, scorehistory: Dict[str, int]) -> None:
        self.id = id
        self.metrics = metrics
        self.name = name
        self.rank = rank
        self.score = score
        self.scorehistory = scorehistory

    @staticmethod
    def from_dict(obj: Any) -> 'Standing':
        assert isinstance(obj, dict)
        id = from_int(obj.get("id"))
        metrics = Metrics.from_dict(obj.get("metrics"))
        name = from_str(obj.get("name"))
        rank = from_int(obj.get("rank"))
        score = from_int(obj.get("score"))
        scorehistory = from_dict(from_int, obj.get("scorehistory"))
        return Standing(id, metrics, name, rank, score, scorehistory)

    def to_dict(self) -> dict:
        result: dict = {}
        result["id"] = from_int(self.id)
        result["metrics"] = to_class(Metrics, self.metrics)
        result["name"] = from_str(self.name)
        result["rank"] = from_int(self.rank)
        result["score"] = from_int(self.score)
        result["scorehistory"] = from_dict(from_int, self.scorehistory)
        return result


class Timer:
    name: str
    time: int

    def __init__(self, name: str, time: int) -> None:
        self.name = name
        self.time = time

    @staticmethod
    def from_dict(obj: Any) -> 'Timer':
        assert isinstance(obj, dict)
        name = from_str(obj.get("name"))
        time = from_int(obj.get("time"))
        return Timer(name, time)

    def to_dict(self) -> dict:
        result: dict = {}
        result["name"] = from_str(self.name)
        result["time"] = from_int(self.time)
        return result


class Welcome:
    generatedat: str
    ismasked: str
    standings: List[Standing]
    timers: List[Timer]

    def __init__(self, generatedat: str, ismasked: str, standings: List[Standing], timers: List[Timer]) -> None:
        self.generatedat = generatedat
        self.ismasked = ismasked
        self.standings = standings
        self.timers = timers

    @staticmethod
    def from_dict(obj: Any) -> 'Welcome':
        assert isinstance(obj, dict)
        generatedat = from_str(obj.get("generated_at"))
        ismasked = from_str(obj.get("is_masked"))
        standings = from_list(Standing.from_dict, obj.get("standings"))
        timers = from_list(Timer.from_dict, obj.get("timers"))
        return Welcome(generatedat, ismasked, standings, timers)

    def to_dict(self) -> dict:
        result: dict = {}
        result["generated_at"] = from_str(self.generatedat)
        result["is_masked"] = from_str(self.ismasked)
        result["standings"] = from_list(lambda x: to_class(Standing, x), self.standings)
        result["timers"] = from_list(lambda x: to_class(Timer, x), self.timers)
        return result


def Welcomefromdict(s: Any) -> Welcome:
    return Welcome.from_dict(s)


def Welcometodict(x: Welcome) -> Any:
    return to_class(Welcome, x)


#path = './'
path = '/var/www/dokuwiki/HaS2/HaS2-scoreboard/version3/'

with open (path + "/" + 'scoreboard.json',"r") as f:
    data = json.load(f)
#    data = json.load(f,object_hook=lambda d: SimpleNamespace(**d))
#    print(data.generated_at, data.is_masked)
#    print(data.standings[1].scorehistory)


result = Welcomefromdict(data)
#print(result.standings[0].id)
for team in result.standings:
    #print(team.id)
    #print(team.name)
    scorelist = team.scorehistory #list
    sorted_scorelist = sorted(scorelist.items())
    scorelist_sorted_dictionary = dict(sorted_scorelist)
    #for score in scorelist_sorted_dictionary.items():
    #    print(score)


loopTerm = True

while(loopTerm):
    loopTerm  = False
    #create new team data from existing format
    newresult = Welcomefromdict(Welcometodict(result))
    #print(newresult.to_dict()) #check json format

    #clear out the score history
    for team in newresult.standings:
        print(team.name)
        scorelist = team.scorehistory
        #clear it out
        team.scorehistory.clear() 
        for k,v in team.scorehistory.items():
            print("helo", k,v)

    with open(path + 'newscoreboard.json','w') as outfile:
        json.dump(Welcometodict(newresult), outfile)
        outfile.close()

    #loop through the original json and 
    #add the scores one by one to the new 
    i = 0
    numScoreEntries = 0
    finalNumScoreEntries = 0
    print(len(result.standings))

    #get max number of scores
    for team in result.standings:
        for k,v in team.scorehistory.items():
            numScoreEntries+=1
            if (finalNumScoreEntries < numScoreEntries):
                finalNumScoreEntries = numScoreEntries
        i+=1
        numScoreEntries = 0

    print("number of score entries %d" %(finalNumScoreEntries))

    for x in range(finalNumScoreEntries):
        i=0
        for team in result.standings:
            scorevalues = team.scorehistory.values()
            scorevalues_list = list(scorevalues)
            keys_list = list(team.scorehistory)
            newscore = scorevalues_list[x]
            newkey = keys_list[x]
            newentry = {newkey:newscore}
            newresult.standings[i].scorehistory.update(newentry)  
            j, k = list(newresult.standings[i].scorehistory.items())[-1]
            i+=1
            print("Done with " + team.name + " score num %d" %x)
        time.sleep(1)


        with open(path + 'newscoreboard.json','w') as outfile:
            json.dump(Welcometodict(newresult), outfile)
            outfile.close()


    print(json.dumps(Welcometodict(newresult), indent=1))

