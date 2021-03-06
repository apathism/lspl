package ru.lspl.text;

import java.util.ArrayList;
import java.util.Map;

import ru.lspl.LsplObject;
import ru.lspl.text.attributes.AttributeContainer;
import ru.lspl.text.attributes.AttributeKey;
import ru.lspl.utils.ArrayMap;

/**
 * Переход в тексте.
 * 
 * @author alno
 */
public class Transition extends LsplObject implements AttributeContainer, TextRange {

	public final Text text;

	public final Node start;

	public final Node end;

	private final int nodeIndex;

	private Map<Integer, Object> attributes = null;

	/**
	 * Выбрать из массива аннотаций те, которые включают позицию
	 * 
	 * @param transitions
	 *            массив аннотаций
	 * @param position
	 *            позиция
	 * @return массив выбранных аннотаций
	 */
	public static Transition[] selectIncludesPosition( Transition[] transitions, int position ) {
		ArrayList<Transition> selectedTransitions = new ArrayList<Transition>();

		for ( Transition t : transitions )
			if ( t.includesPosition( position ) )
				selectedTransitions.add( t );

		return selectedTransitions.toArray( new Transition[selectedTransitions.size()] );
	}

	protected Transition( int id, Text text, int startIndex, int endIndex ) {
		super( id );

		this.text = text;
		this.start = text.getNodes().get( startIndex );
		this.end = text.getNodes().get( endIndex );

		this.nodeIndex = startIndex;
	}

	/**
	 * Проверить, содержит ли аннотация заданную позицию текста позицию
	 * 
	 * @param position
	 *            позиция в тексте
	 * @return true, если содержит
	 */
	public boolean includesPosition( int position ) {
		return start.endOffset < position && end.startOffset > position;
	}

	/**
	 * Сбросить отладочное представление перехода в строку.
	 * 
	 * @return строка, содержащее отладочное представление перехода
	 */
	public native String dump();

	@Override
	protected native void finalize();

	@Override
	public native Object getAttribute( int key );

	@Override
	public Map<Integer, Object> getAttributes() {
		if ( attributes == null ) {
			attributes = new ArrayMap<Object>( AttributeKey.COUNT );

			for ( int i = 1; i < AttributeKey.COUNT; ++i ) {
				if ( i == 12 ) // Ignore text attribute
					continue;

				Object value = getAttribute( i );

				if ( value != null )
					attributes.put( i, value );
			}
		}

		return attributes;
	}

	@Override
	public boolean containsPosition( int index ) {
		return index >= start.endOffset && index <= end.startOffset;
	}

	@Override
	public boolean coincidesWith( TextRange r ) {
		return r != null && r.getText() == text && r.getStartOffset() == start.endOffset && r.getEndOffset() == end.startOffset;
	}

	public boolean coincidesWith( Transition t ) {
		return t != null && t.text == text && t.start == start && t.end == end;
	}

	@Override
	public boolean intersectsWith( TextRange r ) {
		return r != null && r.getText() == text && r.getStartOffset() < end.startOffset && r.getEndOffset() > start.endOffset;
	}

	public boolean intersectsWith( Transition t ) {
		return t != null && t.text == text && t.start.endOffset < end.startOffset && t.end.startOffset > start.endOffset;
	}

	@Override
	public Text getText() {
		return text;
	}

	@Override
	public int getStartOffset() {
		return start.endOffset;
	}

	@Override
	public int getEndOffset() {
		return end.startOffset;
	}

	@Override
	public String getContent() {
		return text.getContent().substring( start.endOffset, end.startOffset );
	}

}