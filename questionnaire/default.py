from .model import *


def defaultExperiment() -> Experiment:
    cfInfo = CFInfo(fw=Firmware(isStock=True, name='', notes=''),
                    isAtRest=True,
                    estimator=Estimator.EKF,
                    deckOption=DeckOptions.OPTION1,
                    notes="notes")

    samplingInfo = SamplingInfo(delay=10, count=10000, rate=20, actionsOption=ActionOptions.OPTION1)

    lpsInfo = LPSInfo(Firmware(isStock=True, name='', notes=''), mode=Mode.TDOA2, which=WhichLPS.OUTER, yaml_file='test.yaml')

    #repo = git.Repo('.', search_parent_directories=True)
    #gitInfo = GitInfo(latest_commit=repo.head.commit.hexsha,
    #                  current_branch=repo.active_branch.name,
    #                  latest_tag=repo.tags[-1].name if repo.tags else None,
    #                  commit_message=repo.head.commit.message,
    #                  author=repo.head.commit.author.name,
    #                  timestamp=repo.head.commit.authored_datetime.now().strftime("%Y-%m-%d_%H-%M-%S"))

    experiment = Experiment(when=Meta.timestamp,
                            where=Where.LAB_DE0302,
                            intent=Intent.NORMAL,
                            lpsCount=1,
                            lpsInfos=[lpsInfo],
                            cfInfo=cfInfo,
                            samplingInfo=samplingInfo)

    return experiment
