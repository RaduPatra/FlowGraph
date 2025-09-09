// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "MovieScene/MovieSceneFlowTemplate.h"
#include "MovieScene/MovieSceneFlowTrack.h"
#include "Nodes/Actor/FlowNode_PlayLevelSequence.h"

#include "Evaluation/MovieSceneEvaluation.h"
#include "IMovieScenePlayer.h"
#include "LevelSequenceActor.h"
#include "Compilation/MovieSceneCompiledDataManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MovieSceneFlowTemplate)

#define LOCTEXT_NAMESPACE "MovieSceneFlowTemplate"

DECLARE_CYCLE_STAT(TEXT("Flow Track Token Execute"), MovieSceneEval_FlowTrack_TokenExecute, STATGROUP_MovieSceneEval);

struct FFlowTrackExecutionToken final : IMovieSceneExecutionToken
{
	FFlowTrackExecutionToken(TArray<FString> InEventNames)
			: EventNames(MoveTemp(InEventNames))
	{
		idTest = FMath::RandRange(0.0f, 100.0f);
	}

	TArray<FString> EventNames;
	bool bEvaluatedUpperBound = false;
	bool bEvaluatedLowerBound = false;
	float idTest = 0.0f;
	

	virtual void Execute(const FMovieSceneContext& Context, const FMovieSceneEvaluationOperand& Operand, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) override
	{
		MOVIESCENE_DETAILED_SCOPE_CYCLE_COUNTER(MovieSceneEval_FlowTrack_TokenExecute)

		for (const FString& EventName : EventNames)
		{
			for (UObject* EventReceiver : Player.GetEventContexts())
			{
				// if (UFlowNode_PlayLevelSequence* FlowNode = Cast<UFlowNode_PlayLevelSequence>(EventReceiver))
				// {
				// 	FlowNode->TriggerEvent(EventName);
				// } 

				UFlowNode* FlowNode = Cast<UFlowNode>(EventReceiver);
				IFlowEventReceiver* FlowEventReceiver = Cast<IFlowEventReceiver>(EventReceiver);
				if (!FlowEventReceiver)
				{
					return;
				}
				ALevelSequenceActor* SequenceActor = FlowEventReceiver->GetSequenceActor();
				if (!SequenceActor)
				{
					return;
				}
				
				ULevelSequence* Sequence = SequenceActor->GetSequence();
				if (!Sequence)
				{
					return;
				}
				//get the current track and section
				const FMovieSceneTrackIdentifier CurrTrackIdentifier = PersistentData.GetTrackKey().TrackIdentifier;
				const int CurrSectionId = PersistentData.GetSectionKey().SectionIndex;

				//get the current frame of the sequence
				const ULevelSequencePlayer* SequencePlayer = SequenceActor->GetSequencePlayer();
				FLevelSequencePlayerSnapshot Snapshot;
				SequencePlayer->TakeFrameSnapshot(Snapshot);
				const FQualifiedFrameTime SnapShotFrame = Snapshot.RootTime;
				double time2 = SnapShotFrame.Time.AsDecimal();
				UE_LOG(LogTemp, Warning, TEXT("All Frame Time: %f, %f"), time2, idTest);
				UE_LOG(LogTemp, Warning, TEXT("Event Name: %s"), *EventName);
				TArray<UMovieSceneTrack*> Tracks = Sequence->GetMovieScene()->GetTracks();
				for (int TrackIdx = 0; TrackIdx < Tracks.Num(); ++TrackIdx)
				{
					const UMovieSceneTrack* Track = Tracks[TrackIdx];
					// UMovieSceneTrack* foundTrack = Sequence->GetMovieScene()->FindTrack(UMovieScene::StaticClass(), Track->GetSignature());
					UMovieSceneCompiledDataManager* CompiledDataManager = Player.GetEvaluationTemplate().GetCompiledDataManager();
					const FMovieSceneCompiledDataID CompiledDataID = CompiledDataManager->GetDataID(Sequence);
					const FMovieSceneEvaluationTemplate* Template = CompiledDataManager->FindTrackTemplate(CompiledDataID);

					//find the track with the same identifier (only way to do this for now since you can't directly get the track id :c)
					const FMovieSceneEvaluationTrack* EvalTrack = Template->FindTrack(CurrTrackIdentifier);
					const UMovieSceneTrack* SourceTrack = EvalTrack->GetSourceTrack();

					//if found track is the same as the current track
					if (SourceTrack == Track)
					{
						TArray<UMovieSceneSection*> Sections = Track->GetAllSections();
						UMovieSceneSection* Section = Sections[CurrSectionId];
						const UMovieSceneFlowRepeaterSection* RepeaterFlowSection = Cast<UMovieSceneFlowRepeaterSection>(Section);
						if (!RepeaterFlowSection)
						{
							break;
						}

						bool preroll = Context.IsPreRoll();
						UE_LOG(LogTemp, Warning, TEXT("Preroll: %d"), preroll);
						bool silent = Context.IsSilent();
						UE_LOG(LogTemp, Warning, TEXT("Preroll: %d"), silent);

						UE_LOG(LogTemp, Warning, TEXT("Message"));
						const int TicksPerFrame =Sequence->GetMovieScene()->GetTickResolution().Numerator / Sequence->GetMovieScene()->GetDisplayRate().Numerator;
						FFrameNumber a = SnapShotFrame.Time.GetFrame();
						TRange<FFrameNumber> SectionRange = RepeaterFlowSection->GetRange();
						
						TRangeBound<FFrameNumber> UpperBound = SectionRange.GetUpperBound();
						const FFrameNumber UpperVal = UpperBound.GetValue().Value / TicksPerFrame;

						TRangeBound<FFrameNumber> LowerBound = SectionRange.GetLowerBound();
						const FFrameNumber LowVal = LowerBound.GetValue().Value / TicksPerFrame;

						FQualifiedFrameTime RangeQualifiedTimeLower = FQualifiedFrameTime(LowVal, SnapShotFrame.Rate);
						FQualifiedFrameTime RangeQualifiedTimeUpper = FQualifiedFrameTime(UpperVal, SnapShotFrame.Rate);

						if (LowVal == SnapShotFrame.Time.FrameNumber && !bEvaluatedLowerBound)
						{
							bEvaluatedLowerBound = true;
							double time = SnapShotFrame.Time.AsDecimal();
							UE_LOG(LogTemp, Warning, TEXT("Frame Time: %f, %f"), time, idTest);
							FlowEventReceiver->TriggerSectionBeginEvent(EventName);
							return;
						}
						else if (UpperVal == SnapShotFrame.Time.FrameNumber && !bEvaluatedUpperBound)
						{
							bEvaluatedUpperBound = true;
							double time = SnapShotFrame.Time.AsDecimal();
							UE_LOG(LogTemp, Warning, TEXT("Frame Time: %f, %f"), time, idTest);
							FlowEventReceiver->TriggerSectionFinishEvent(EventName);
							return;
						}

						if (bEvaluatedLowerBound || bEvaluatedUpperBound)
						{
							return;
						}
					}
				}

				//for cinematic dialogue, should prob only trigger 'trigger' events here, not section events. Unless somehow we want to do something every frame during the section.
				//maybe a custom dialogue Flow exec token would be a better idea
				FlowEventReceiver->TriggerEvent(EventName);
				FlowEventReceiver->TriggerEvent(EventName, Context, PersistentData, Player);
			}
		}
	}
};


FMovieSceneFlowTriggerTemplate::FMovieSceneFlowTriggerTemplate(const UMovieSceneFlowTriggerSection& Section, const UMovieSceneFlowTrack& Track)
	: FMovieSceneFlowTemplateBase(Track, Section)
{
	const TMovieSceneChannelData<const FString> EventData = Section.StringChannel.GetData();
	const TArrayView<const FFrameNumber> Times = EventData.GetTimes();
	const TArrayView<const FString> EntryPoints = EventData.GetValues();

	EventTimes.Reserve(Times.Num());
	EventNames.Reserve(Times.Num());

	for (int32 Index = 0; Index < Times.Num(); ++Index)
	{
		EventTimes.Add(Times[Index]);
		EventNames.Add(EntryPoints[Index]);
	}
}

void FMovieSceneFlowTriggerTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& SweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
	// Don't allow events to fire when playback is in a stopped state. This can occur when stopping 
	// playback and returning the current position to the start of playback. It's not desirable to have 
	// all the events from the last playback position to the start of playback be fired.
	if (Context.GetStatus() == EMovieScenePlayerStatus::Stopped || Context.IsSilent())
	{
		return;
	}

	const bool bBackwards = Context.GetDirection() == EPlayDirection::Backwards;

	if ((!bBackwards && !bFireEventsWhenForwards) || (bBackwards && !bFireEventsWhenBackwards))
	{
		return;
	}

	TArray<FString> EventsToTrigger;

	if (bBackwards)
	{
		// Trigger events backwards
		for (int32 KeyIndex = EventTimes.Num() - 1; KeyIndex >= 0; --KeyIndex)
		{
			FFrameNumber Time = EventTimes[KeyIndex];
			if (!EventNames[KeyIndex].IsEmpty() && SweptRange.Contains(Time))
			{
				EventsToTrigger.Add(EventNames[KeyIndex]);
			}
		}
	}
	else
	{
		// Trigger events forwards
		for (int32 KeyIndex = 0; KeyIndex < EventTimes.Num(); ++KeyIndex)
		{
			FFrameNumber Time = EventTimes[KeyIndex];
			if (!EventNames[KeyIndex].IsEmpty() && SweptRange.Contains(Time))
			{
				EventsToTrigger.Add(EventNames[KeyIndex]);
			}
		}
	}

	if (EventsToTrigger.Num())
	{
		ExecutionTokens.Add(FFlowTrackExecutionToken(MoveTemp(EventsToTrigger)));
	}
}

FMovieSceneFlowRepeaterTemplate::FMovieSceneFlowRepeaterTemplate(const UMovieSceneFlowRepeaterSection& Section, const UMovieSceneFlowTrack& Track)
	: FMovieSceneFlowTemplateBase(Track, Section)
	, EventName(Section.EventName)
{
}

void FMovieSceneFlowRepeaterTemplate::EvaluateSwept(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const TRange<FFrameNumber>& SweptRange, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
	const bool bBackwards = Context.GetDirection() == EPlayDirection::Backwards;
	const FFrameNumber CurrentFrame = bBackwards ? Context.GetTime().CeilToFrame() : Context.GetTime().FloorToFrame();

	// Don't allow events to fire when playback is in a stopped state. This can occur when stopping 
	// playback and returning the current position to the start of playback. It's not desirable to have 
	// all the events from the last playback position to the start of playback be fired.
	if (EventName.IsEmpty() || !SweptRange.Contains(CurrentFrame) || Context.GetStatus() == EMovieScenePlayerStatus::Stopped || Context.IsSilent())
	{
		return;
	}

	if ((!bBackwards && bFireEventsWhenForwards) || (bBackwards && bFireEventsWhenBackwards))
	{
		ExecutionTokens.Add(FFlowTrackExecutionToken({EventName}));
	}
}

#undef LOCTEXT_NAMESPACE
